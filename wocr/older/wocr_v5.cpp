// cl -std:c++20 -EHsc wocr.cpp /link OleAut32.lib ole32.lib

#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <cmath>

#include <winrt/base.h>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Media.Ocr.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>

namespace fs = std::filesystem;

//-----------------------------------------------------------------------------

using winrt::Windows::Graphics::Imaging::SoftwareBitmap;
using winrt::Windows::Media::Ocr::OcrEngine;

//-----------------------------------------------------------------------------

SoftwareBitmap LoadBitmap(const fs::path & imagePath);

std::wstring RecognizeText(
    OcrEngine const & engine,
    SoftwareBitmap const & bitmap);

void SaveTextFile(
    const fs::path & outputPath,
    std::wstring_view text);

//-----------------------------------------------------------------------------

int main(int argc, char * argv[])
{
    winrt::init_apartment();

    if (argc != 2)
    {
        std::cerr << "Usage: ocr <image-file>\n";
        return 1;
    }

    try
    {
        fs::path imagePath = fs::absolute(argv[1]);

        if (!fs::exists(imagePath))
        {
            std::cerr << "File not found.\n";
            return 1;
        }

        OcrEngine engine = OcrEngine::TryCreateFromUserProfileLanguages();

        if (!engine)
        {
            std::cerr << "Unable to create OCR engine.\n";
            return 1;
        }

        SoftwareBitmap bitmap = LoadBitmap(imagePath);

        std::wstring text = RecognizeText(engine, bitmap);

        fs::path outputPath = imagePath;
        outputPath.replace_extension(".txt");

        SaveTextFile(outputPath, text);

        std::cout << outputPath.filename().string() << "\n";
    }
    catch (winrt::hresult_error const & ex)
    {
        std::wcerr << L"WinRT error: " << ex.message().c_str() << L'\n';
        return 2;
    }
    catch (std::exception const & ex)
    {
        std::cerr << ex.what() << '\n';
        return 3;
    }

    return 0;
}


// =======================================


void SaveTextFile(
    const fs::path & outputPath,
    std::wstring_view text)
{
    std::ofstream out(outputPath, std::ios::binary);

    if (!out)
    {
        throw std::runtime_error(
            "Unable to create output file: " +
            outputPath.string());
    }

    if ( false ) // no BOM need
    {
        constexpr unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
        out.write(reinterpret_cast<const char *>(bom), sizeof(bom));
    }

    // Convert UTF-16 (wstring) -> UTF-8
    std::string utf8 = winrt::to_string(text);

    out.write(utf8.data(), static_cast<std::streamsize>(utf8.size()));

    if (!out)
    {
        throw std::runtime_error(
            "Error writing output file: " +
            outputPath.string());
    }
}

SoftwareBitmap LoadBitmap(const fs::path & imagePath)
{
    using namespace winrt;
    using namespace Windows::Storage;
    using namespace Windows::Storage::Streams;
    using namespace Windows::Graphics::Imaging;

    // Open the file as a WinRT StorageFile.
    StorageFile file =
        StorageFile::GetFileFromPathAsync(imagePath.wstring()).get();

    // Open a read-only stream.
    IRandomAccessStream stream =
        file.OpenReadAsync().get();

    // Create a decoder for the image format.
    BitmapDecoder decoder =
        BitmapDecoder::CreateAsync(stream).get();

    // Decode the image into a SoftwareBitmap.
    SoftwareBitmap bitmap =
        decoder.GetSoftwareBitmapAsync(
            BitmapPixelFormat::Bgra8,
            BitmapAlphaMode::Ignore).get();

    return bitmap;
}

std::wstring RecognizeText_ver1(
    OcrEngine const & engine,
    SoftwareBitmap const & bitmap)
{
    using namespace winrt;
    using namespace Windows::Media::Ocr;

    OcrResult result = engine.RecognizeAsync(bitmap).get();

    // just text
    //return std::wstring(result.Text());

    std::wstring text;

    bool first = true;

    for (auto const & line : result.Lines())
    {
        if (!first)
        {
            text += L'\n';
        }

        text += line.Text();

        first = false;
    }

    return text;
}

std::wstring RecognizeText(
    OcrEngine const & engine,
    SoftwareBitmap const & bitmap)
{
    auto result = engine.RecognizeAsync(bitmap).get();

    struct WordInfo
    {
        float x;
        float width;
        std::wstring text;
    };

    struct LineInfo
    {
        float y;
        float height;
        std::vector<WordInfo> words;
    };

    std::vector<LineInfo> lines;

    float totalCharWidth = 0.0f;
    size_t totalCharacters = 0;
    float totalHeight = 0.0f;
    size_t wordCount = 0;


    for (auto const & line : result.Lines())
    {
        LineInfo info {};

        float minY = std::numeric_limits<float>::max();
        float maxY = 0.0f;

        for (auto const & word : line.Words())
        {
            auto rect = word.BoundingRect();

            std::wstring wordText(word.Text());

            info.words.push_back(
            {
                rect.X,
                rect.Width,
                wordText
            });

            minY = std::min(minY, rect.Y);
            maxY = std::max(maxY, rect.Y + rect.Height);

            totalCharWidth += rect.Width;
            totalCharacters += wordText.length();
            totalHeight += rect.Height;
            ++wordCount;
        }

        info.y = minY;
        info.height = maxY - minY;

        lines.push_back(std::move(info));
    }

    float averageCharWidth =
        totalCharWidth / static_cast<float>(totalCharacters);

    float averageLineHeight = 0.0f;

    if (wordCount > 0)
    {
        averageLineHeight = totalHeight / static_cast<float>(wordCount);
    }

    float yThreshold = averageLineHeight * 0.5f;

    std::sort(
        lines.begin(),
        lines.end(),
        [yThreshold](auto const & a, auto const & b)
    {
        if (std::abs(a.y - b.y) > yThreshold)
            return a.y < b.y;

        //return a.x < b.x;
        return false; // om??? why not above ??
    });


    std::wstring output;

    for (auto const & line : lines)
    {
        if (!output.empty())
            output += L'\n';

        float lineStartX = line.words.front().x;

        float currentPosition = 0 * lineStartX;

        for (auto const & word : line.words)
        {
            int spaces =
                static_cast<int>(
                    (word.x - currentPosition) /
                    averageCharWidth);

            if (spaces > 0)
                output.append(spaces, L' ');

            output += word.text;

            currentPosition =
                word.x + word.width;
        }
    }


    return output;
}

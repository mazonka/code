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
#include <winrt/Windows.Globalization.h>

#include <winrt/Windows.Data.Pdf.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Graphics.Imaging.h>

#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>

#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>

namespace fs = std::filesystem;

//-----------------------------------------------------------------------------

using winrt::Windows::Graphics::Imaging::SoftwareBitmap;
using winrt::Windows::Media::Ocr::OcrEngine;

//-----------------------------------------------------------------------------

SoftwareBitmap LoadBitmap(const fs::path & imagePath);
SoftwareBitmap LoadPdfPage(const fs::path & imagePath);

std::wstring RecognizeText(
    OcrEngine const & engine,
    SoftwareBitmap const & bitmap);

void SaveTextFile(
    const fs::path & outputPath,
    std::wstring_view text);

void SaveBitmapAsPng(
    winrt::Windows::Graphics::Imaging::SoftwareBitmap const & bitmap,
    std::filesystem::path const & filename);

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

        //OcrEngine engine = OcrEngine::TryCreateFromUserProfileLanguages();
        auto language = winrt::Windows::Globalization::Language(L"en-US");
        auto engine = winrt::Windows::Media::Ocr::OcrEngine::TryCreateFromLanguage(language);

        if (!engine)
        {
            std::cerr << "Unable to create OCR engine.\n";
            return 1;
        }

        SoftwareBitmap bitmap = nullptr;
        if (imagePath.extension() == L".pdf"
                || imagePath.extension() == L".PDF" )
        {
            bitmap = LoadPdfPage(imagePath);
            fs::path pngPath = imagePath;
            pngPath.replace_extension(".png");
            SaveBitmapAsPng(bitmap, pngPath);
        }
        else
        {
            bitmap = LoadBitmap(imagePath);
        }


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

    struct LineInfo
    {
        float y;
        float x;
        float height;
        float width;
        std::wstring text;
    };

    std::vector<LineInfo> lines;

    float totalHeight = 0.0f;
    size_t wordCount = 0;

    for (auto const & line : result.Lines())
    {
        float minX = std::numeric_limits<float>::max();
        float minY = std::numeric_limits<float>::max();
        float maxY = 0.0f;
        float maxX = 0.0f;

        for (auto const & word : line.Words())
        {
            auto rect = word.BoundingRect();

            minX = std::min(minX, rect.X);
            minY = std::min(minY, rect.Y);
            maxY = std::max(maxY, rect.Y + rect.Height);
            maxX = std::max(maxX, rect.X + rect.Width);

            totalHeight += rect.Height;
            ++wordCount;
        }

        lines.push_back(
        {
            minY,
            minX,
            maxY - minY,
            maxX - minX,
            std::wstring(line.Text())
        });
    }

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

        return a.x < b.x;
    });

    // calc w
    float wav = 0;
    int iwav = 0;
    for (auto const & line : lines)
    {
        auto lt = line.text;
        auto sz = lt.size();
        if ( sz )
        {
            wav += line.width / sz;
            ++iwav;
        }
    }

    float w = iwav ? wav / iwav : 1e6;

    std::wstring text;

    for (auto const & line : lines)
    {
        if (!text.empty())
            text += L'\n';

        auto lt = line.text;
        auto sz = lt.size();
        if ( sz )
        {
            auto x = line.x;
            while ((x -= w) > 0) text += ' ';
        }

        text += lt;
    }

    return text;
}

winrt::Windows::Graphics::Imaging::SoftwareBitmap
LoadPdfPage(std::filesystem::path const & pdfPath)
{
    using namespace winrt;
    using namespace Windows::Data::Pdf;
    using namespace Windows::Storage;
    using namespace Windows::Storage::Streams;
    using namespace Windows::Graphics::Imaging;

    auto file =
        StorageFile::GetFileFromPathAsync(
            pdfPath.wstring()).get();

    auto document =
        PdfDocument::LoadFromFileAsync(file).get();

    if (document.PageCount() == 0)
    {
        throw std::runtime_error("PDF contains no pages");
    }

    auto page = document.GetPage(0);

    auto stream =
        InMemoryRandomAccessStream();

    page.RenderToStreamAsync(stream).get();

    stream.Seek(0);

    auto decoder =
        BitmapDecoder::CreateAsync(stream).get();

    auto bitmap =
        decoder.GetSoftwareBitmapAsync().get();

    // OCR prefers BGRA8
    if (bitmap.BitmapPixelFormat() != BitmapPixelFormat::Bgra8)
    {
        bitmap =
            SoftwareBitmap::Convert(
                bitmap,
                BitmapPixelFormat::Bgra8,
                BitmapAlphaMode::Ignore);
    }

    return bitmap;
}


void SaveBitmapAsPng(
    winrt::Windows::Graphics::Imaging::SoftwareBitmap const & bitmap,
    std::filesystem::path const & filename)
{
    using namespace winrt;
    using namespace Windows::Graphics::Imaging;
    using namespace Windows::Storage;

    auto folder =
        StorageFolder::GetFolderFromPathAsync(
            filename.parent_path().wstring()).get();

    auto file =
        folder.CreateFileAsync(
            filename.filename().wstring(),
            CreationCollisionOption::ReplaceExisting).get();

    auto stream =
        file.OpenAsync(
            FileAccessMode::ReadWrite).get();

    auto encoder =
        BitmapEncoder::CreateAsync(
            BitmapEncoder::PngEncoderId(),
            stream).get();

    auto converted =
        SoftwareBitmap::Convert(
            bitmap,
            BitmapPixelFormat::Bgra8,
            BitmapAlphaMode::Ignore);

    encoder.SetSoftwareBitmap(converted);

    encoder.FlushAsync().get();
}

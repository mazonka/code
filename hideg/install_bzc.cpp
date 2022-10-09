#include "crun.h"


void cmain()
{
    auto app = std::getenv("APP");
    cout << "APP = " << app << '\n';
    fs::path bin = app;
    bin = bin / "app" / "bin";
    cout << "BIN = " << bin.string() << '\n';

    if ( !fs::is_directory(bin) ) throw bin.string() + " not accessible";

    fs::path f = "bzc.exe";

    if ( !fs::is_regular_file(f) )
    {
        std::system("cl -Ox -EHsc -std:c++17 bzc.cpp");
        fs::remove("bzc.obj");
        throw "run again";
        throw "Build executable";
    }

    auto binf = bin / f;
    if ( fs::is_regular_file(binf) ) fs::remove(binf);
    fs::copy(f, bin / f, fs::copy_options::overwrite_existing);
    if ( !fs::is_regular_file(binf) ) throw "copy failed";
    fs::remove("bzc.exe");
}
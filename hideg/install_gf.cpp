#include "ccrun.h"


void cmain()
{
    auto app = std::getenv("APP");
    cout << "APP = " << app << '\n';
    fs::path bin = app;
    bin = bin / "app" / "bin";
    cout << "BIN = " << bin.string() << '\n';

    if ( !fs::is_directory(bin) ) throw bin.string() + " not accessible";

    fs::path f1 = "bzc.exe";
    fs::path f2 = "gf.exe";

    //if ( !fs::is_regular_file(f1) || !fs::is_regular_file(f2) )
    if ( !fs::is_regular_file(f2) )
    {
        std::system("make");
        if ( !fs::is_regular_file(f2) ) throw "Failed to build executable";
        //if ( !fs::is_regular_file(f1) ) throw "Failed to build executable";
    }

    cout << '\n';
    //for ( auto f : {f1, f2} ) // BOTH
    for ( auto f : {f2} ) // ONLY gf
    {
        auto binf = bin / f;
        if ( fs::is_regular_file(binf) ) fs::remove(binf);
        fs::copy(f, bin / f, fs::copy_options::overwrite_existing);
        if ( !fs::is_regular_file(binf) ) throw "copy failed";
        cout << "installed : " << f.string() << "\n";
    }

    //fs::remove("bzc.exe");
    fs::remove("gf.exe");
    fs::remove(".gf.key");
}
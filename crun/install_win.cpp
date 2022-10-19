#define CRUN

#ifdef CRUN

#include "crun.h"
void cmain()
{
    string app = std::getenv("APP");
    app += "\\run\\crun.bat";
    std::ofstream(app) << "@c:\\crun\\crun.exe %*\n";

    string dir = "c:/crun";

    fs::create_directories(dir);

    auto cp = [](string f, fs::path t)
    {
        if ( !fs::exists(f) ) throw "No file " + f;
        fs::copy(f, t / f, fs::copy_options::overwrite_existing);
    };

    auto cpto = [&cp](string f) { cp(f, "c:/crun"); };

    cpto("compile.crun");
    cpto("crun.exe");
    cpto("crun.h");

    cout << "Installed in " << app << " and " << dir << '\n';
}

#else

#include <filesystem>
#include <fstream>
#include <iostream>

using namespace std;
namespace fs = std::filesystem;

int main()
try
{
    string app = std::getenv("APP");
    app += "\\run\\crun.bat";
    ofstream(app) << "@c:\\crun\\crun.exe %*\n";

    fs::create_directories("c:/crun");

    auto cp = [](string f, fs::path t)
    {
        fs::copy(f, t / f, fs::copy_options::overwrite_existing);
    };

    auto cpto = [&cp](string f) { cp(f, "c:/crun"); };

    cpto("compile.crun");
    cpto("crun.exe");
}

catch (int e)
{
    cout << "error (int): " << e << "\n";
    return 1;
}
catch (string e)
{
    cout << "error: " << e << "\n";
    return 1;
}
catch (const char * e)
{
    cout << "error: " << e << "\n";
    return 1;
}
catch (std::exception & e)
{
    cout << "error (std): " << e.what() << "\n";
    return 1;
}
catch (...)
{
    cout << "Unknown exception\n";
    return 1;
}

#endif

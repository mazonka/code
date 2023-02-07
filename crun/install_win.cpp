#define CCRUN

#ifdef CCRUN

#include "ccrun.h"
void cmain()
{
    string bat = std::getenv("APP");
    bat += "\\run\\ccrun_win.bat";
    std::ofstream(bat) << "@c:\\ccrun\\ccrun.exe %*\n";

    string dir = "c:/ccrun";

    fs::create_directories(dir);

    auto cp = [](string f, fs::path t)
    {
        if ( !fs::exists(f) ) throw "No file " + f;
        fs::copy(f, t / f, fs::copy_options::overwrite_existing);
    };

    auto cpto = [&cp](string f) { cp(f, "c:/ccrun"); };

    cpto("compile.ccrun");
    cpto("ccrun.exe");
    cpto("ccrun.h");

    cout << "Installed in " << bat << " and " << dir << '\n';
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
    string bat = std::getenv("APP");
    bat += "\\run\\ccrun_win.bat";
    ofstream(bat) << "@c:\\ccrun\\ccrun.exe %*\n";

    fs::create_directories("c:/ccrun");

    auto cp = [](string f, fs::path t)
    {
        fs::copy(f, t / f, fs::copy_options::overwrite_existing);
    };

    auto cpto = [&cp](string f) { cp(f, "c:/ccrun"); };

    cpto("compile.ccrun");
    cpto("ccrun.exe");
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

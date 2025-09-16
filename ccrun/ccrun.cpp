#include <filesystem>
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <string>
#include <cstdlib>

using namespace std;
namespace fs = std::filesystem;

bool replaceAll(string & s, const string & r, const string & to)
{
    bool found = false;
    while (1)
    {
        size_t i = s.find(r);
        if ( i == string::npos ) return found;
        s.replace(i, r.size(), to);
        found = true;
    }
    throw 0; // never
}

struct G
{
    bool mock = false;
    bool local = true;

    fs::path exe, root, base, cwd, binpath, compilepath;
    fs::path filebin;
    string filecpp, workname, fileexe, cmd;
    unsigned long long tm_cpp = 0, tm_bin = 0;
    std::vector<string> compile;

    void print()
    {
        cout << "version 230222\n";
        cout << "ccrun executable : " << exe.string() << '\n';
        cout << "ccrun location   : [" << root.string() << "]\n";
        cout << "ccrun cache      : " << base.string() << '\n';
        cout << "current path     : " << cwd.string() << '\n';
        cout << "binary location  : " << binpath.string() << '\n';
        cout << "compiling cfg    : " << compilepath.string() << '\n';

        if (!filecpp.empty())
        {
            cout << "input file       : " << filecpp << '\n';
            cout << "working name     : " << workname << '\n';
            cout << "executable name  : " << fileexe << '\n';
            cout << "binary filepath  : " << filebin.string() << '\n';
            cout << "file timestamp   : " << tm_cpp << '\n';
            cout << "bin timestamp    : " << tm_bin << '\n';
        }

        if ( !compile.empty() )
            for ( auto c : compile ) cout << "compile : " << c << '\n';
        cout << "command line    : [" << cmd << "]\n";
    }

    void init(string av0, string fil);

    bool proceed() { return !mock && !filecpp.empty(); }
};


fs::path fix_root()
{
    auto phome = std::getenv("HOME");
    if ( !phome )
    {
        auto pcrun = std::getenv("CCRUN");
        if ( !pcrun )
            throw "ccrun location unknown; neither HOME(env), nor CCRUN(env) are set";

        return pcrun;
    }
    fs::path r = string(phome);
    return r / "ccrun";
}

void G::init(string av0, string fil)
{
    exe = av0;
    root = exe;
    root.remove_filename();
    if ( root.empty() ) root = fix_root();
    base = root;
    base /= "cache";
    cwd = fs::current_path();

    auto absp2str = [](const fs::path & p) -> string
    {
        string sd = p.string();
        replaceAll(sd, ":", "");
        while ( sd.size() && sd[0] == '/' ) sd = sd.substr(1);
        return sd;
    };

    ///string scwd = cwd.string();
    ///replaceAll(scwd, ":", "");
    ///while ( scwd.size() && scwd[0] == '/' ) scwd = scwd.substr(1);
    string scwd = absp2str(cwd);

    filecpp = fil;
    if ( filecpp[0] == '!') mock = true;

    auto pcpp = fs::path(filecpp);

    ///cout << "AAA base=[" << base << "] scwd=[" << scwd << "] pcpp.filename=["
    ///     << (pcpp.filename().string()) << "] filecpp=[" << (filecpp) << "] pcpp.parent=["
    ///     << (pcpp.parent_path().string()) << "]\n";

    binpath = base / scwd;
    if ( pcpp.filename().string() != filecpp ) local = false;
    ///if ( !local ) binpath /= pcpp.parent_path();
    if ( !local )
    {
        auto pp = pcpp.parent_path();
        if ( !pp.is_absolute() ) binpath /= pp;
        else
        {
            ///string s = pp.string();
            ///replaceAll(s, ":", "");
            ///while ( s.size() && s[0] == '/' ) s = s.substr(1);
            string s = absp2str(pp);
            binpath = base / s;
        }
    }

    if ( !mock && !fs::exists(pcpp) )
    {
        cout << "Usage: ccrun [!] name.cpp [args]\n";
        throw "No file " + filecpp;
    }

    string ext = pcpp.extension().string();
    if ( ext != ".cpp") throw "input file " + filecpp + " - bad extension [" + ext + "]";
    workname = pcpp.stem().string();

    compilepath = "compile.ccrun";
    if ( !fs::exists(compilepath) ) compilepath = root / compilepath;
    if ( !fs::exists(compilepath) )
        throw "No config compile file " + compilepath.string();

    fileexe = workname + ".exe";
    filebin = binpath / fileexe;


    auto gettm = [](fs::path f) -> auto
    {
        return 1ull * fs::last_write_time(f).time_since_epoch().count();
    };

    if ( fs::exists(pcpp) ) tm_cpp = gettm(pcpp);
    if ( fs::exists(filebin) ) tm_bin = gettm(filebin);

    std::ifstream in(compilepath);
    for ( string line; std::getline(in, line); )
    {
        if ( line.empty() ) continue;
        if ( line[0] == ':' || line[0] == '#' ) continue;
        replaceAll(line, "$N", workname);
        replaceAll(line, "$I", root.string());
        compile.push_back(line);
    }

    cmd = filebin.string();
}

G g;

int main(int ac, const char * av[])
try
{
    string file = ac >= 2 ? av[1] : "";
    int i2 = 2;
    if ( file == "!" && ac >= 3 )
    {
        g.mock = true;
        i2 = 3;
        file = av[2];
    }

    while ( file.size() > 2 && file.substr(0, 2) == "./" )
        file = file.substr(2);

    g.init(av[0], file );

    for ( int i = i2; i < ac; i++ ) (g.cmd += " ") += av[i];

    if ( !g.proceed() )
    {
        g.print();
        return 0;
    }

    fs::create_directories(g.binpath);
    if ( g.tm_cpp > g.tm_bin )
    {
        ///cout << " AAA tm_cpp=" << g.tm_cpp << " tm_bin=" << g.tm_bin << " local=" << g.local << "\n";
        if ( !g.local )
        {
            throw "recompile at the origin (this req maybe removed in future)"
            "\ni.e. run ccrun without relative path"
            "\ne.g. ccrun a.cpp; NOT ccrun ../a.cpp";
        }

        fs::remove(g.filebin);
        cout << "compiling...\n";
        for ( auto c : g.compile )
        {
            int err = std::system(c.c_str());
            if ( err ) return err;
        }

        fs::rename(g.workname + ".exe", g.filebin);
    }
    //else cout<<"no need to compile\n";

    return std::system(g.cmd.c_str());
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


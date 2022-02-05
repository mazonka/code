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
    fs::path exe, root, base, cwd, binpath, compilepath;
    fs::path filebin;
    string filecpp, workname, fileexe, cmd;
    unsigned long long tm_cpp = 0, tm_bin = 0;
    std::vector<string> compile;

    void print()
    {
        cout << "crun executable : " << exe.string() << '\n';
        cout << "crun location   : " << root.string() << '\n';
        cout << "crun cache      : " << base.string() << '\n';
        cout << "current path    : " << cwd.string() << '\n';
        cout << "binary location : " << binpath.string() << '\n';
        cout << "compiling cfg   : " << compilepath.string() << '\n';

        if (!filecpp.empty())
        {
            cout << "input file      : " << filecpp << '\n';
            cout << "working name    : " << workname << '\n';
            cout << "executable name : " << fileexe << '\n';
            cout << "binary filepath : " << filebin.string() << '\n';
            cout << "file timestamp  : " << tm_cpp << '\n';
            cout << "bin timestamp   : " << tm_bin << '\n';
        }

        if ( !compile.empty() )
            for ( auto c : compile ) cout << "compile : " << c << '\n';
        cout << "command line   : [" << cmd << "]\n";
    }

    void init(string av0, string fil);

    bool proceed()
    {
        if (filecpp.empty() ) return false;
        if ( filecpp[0] == '!')
        {
            //filecpp = filecpp.substr(1);
            return false;
        }
        return true;
    }
};


void G::init(string av0, string fil)
{
    exe = av0;
    root = exe;
    root.remove_filename();
    base = root;
    base /= "cache";
    cwd = fs::current_path();
    string scwd = cwd.string();
    replaceAll(scwd, ":", "");
    binpath = base / scwd;
    filecpp = fil;

    auto pfile = fs::path(filecpp);
    string ext = pfile.extension().string();
    if ( ext != ".cpp") throw "input file " + filecpp + " - bad extension [" + ext + "]";
    workname = pfile.stem().string();

    compilepath = root / "compile";
    fileexe = workname + ".exe";
    filebin = binpath / fileexe;

    if ( !fs::exists(compilepath) )
        throw "No config compile file " + compilepath.string() + ". Please, create one";

    if ( !fs::exists(filecpp) ) throw "No file " + filecpp;

    auto gettm = [](fs::path f) -> auto
    {
        return 1ull * fs::last_write_time(f).time_since_epoch().count();
    };

    tm_cpp = gettm(filecpp);
    if ( fs::exists(filebin) ) tm_bin = gettm(filebin);

    std::ifstream in(compilepath);
    for ( string line; std::getline(in, line); )
    {
        if ( line.empty() ) continue;
        if ( line[0] == ':' || line[0] == '#' ) continue;
        replaceAll(line, "$$", workname);
        compile.push_back(line);
    }

	cmd = filebin.string();
}

G g;

int main(int ac, const char * av[])
try
{
    g.init(av[0], ac >= 2 ? av[1] : "" );
	for( int i=2; i<ac; i++ ) (g.cmd += " ") += av[i];

    if ( !g.proceed() )
    {
        g.print();
        return 0;
    }

    fs::create_directories(g.binpath);
    if ( g.tm_cpp > g.tm_bin )
    {
		fs::remove(g.filebin);
        cout << "compiling...\n";
        for ( auto c : g.compile )
        {
            int err = std::system(c.c_str());
            if ( err ) return err;
        }

		fs::rename(g.workname+".exe",g.filebin);
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


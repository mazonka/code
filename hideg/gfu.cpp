#include "olu.h"
#include "hash.h"

#include "gfu.h"

fs::path g::gfexe; // arg0
int g::depth = -1;
fs::path g::keyfile; // key file path
string g::sysuid;
string g::keyfilename; // filename only
string g::hkey; // key hex hash

fs::path g::dotgf; // ".gf" inited in sync
bool g::recursive_mode = false;
fs::path g::cwd, g::root_cwd;



string gfu::fileHash(string file)
{
    string body = ol::file2str(file);
    string hash = ha::hashHex(body);
    return hash;
}

void gfu::find_depth()
{
    if ( g::depth >= 0 ) return;
    auto fnd = []() -> int
    {
        string sp = ".";
        fs::path p(sp);
        for (int i = 0; i < 1000; i++)
        {
            fs::path cp(sp += "/..");
            if ( fs::equivalent(p, cp) ) return i;
            p = cp;
        }

        throw "Error in depth";
    };

    g::depth = fnd();
}


void gfu::find_key()
{
    auto cwd = fs::current_path();

    fs::path r;

    for (int i = g::depth; i >= 0; i--)
    {
        auto cp = fs::current_path();
        auto cf = cp / g::keyfilename;
        //cout << cf.string() << '\n';
        if (fs::is_regular_file(cf)) { r = cf; break; }
        fs::current_path("..");
    }

    fs::current_path(cwd);
    g::keyfile = r;
}

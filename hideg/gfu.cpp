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

int g::keepfile = 0; // 0-auto, 1-keep, 2-discard


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


string gfu::dkey(int dlev)
{
    string pw = g::hkey;
    if ( pw.empty() ) never;

    // untie from bzc
    {
        for_i(dlev) pw = ha::hashHex(pw);
        pw = ha::hashHex(pw + g::hkey);
    }

    return pw;
}

void gfu::zpaq_unpack(string file, bool withkey)
{
    int k = 0;

    if (withkey)
    {
        string pw = dkey(2);
        k = ol::zpaq(file, false, pw);
    }
    else
    {
        k = ol::zpaq(file, false, "");
    }

    if ( k ) throw "zpaq failed";
}


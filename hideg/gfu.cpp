#include "gfu.h"

int g_depth = -1;
fs::path g_keyfile; // key file path
string g_sysuid;
string g_keyfilename; // filename only
string g_hkey; // key hex hash

void find_depth()
{
    if ( g_depth >= 0 ) return;
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

    g_depth = fnd();
}


void find_key()
{
    auto cwd = fs::current_path();

    fs::path r;

    for (int i = g_depth; i >= 0; i--)
    {
        auto cp = fs::current_path();
        auto cf = cp / g_keyfilename;
        //cout << cf.string() << '\n';
        if (fs::is_regular_file(cf)) { r = cf; break; }
        fs::current_path("..");
    }

    fs::current_path(cwd);
    g_keyfile = r;
}

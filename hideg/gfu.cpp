#include <iostream>

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
ivec<string> g::ignore;
bool g::silent = false;


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
        k = gfu::zpaq(file, false, pw);
    }
    else
    {
        k = gfu::zpaq(file, false, "");
    }

    if ( k ) throw "zpaq failed";
}

bool gfu::ignored(string name)
{
    if ( name.empty() ) never;
    for ( auto i : g::ignore )
    {
        if ( i == "." && name[0] == '.' ) return true;
        if ( i == name ) return true;
    }
    return false;
}


int gfu::bzip(string file, bool pck, bool keep)
{
    auto fsz = fs::file_size(file);
    string cmd = "bzip2 ";
    if ( fsz > 5000000 ) cmd += "-v ";
    if ( !pck ) cmd += "-d ";
    if ( keep) cmd += "-k ";
    cmd += file;
    std::cout << std::flush; std::cerr << std::flush;
    int rsys = std::system(cmd.c_str());
    return rsys;
}

int gfu::zpaq(string file, bool pck, string key)
{
    string cmd = "zpaq ";
    if ( pck ) cmd += "a ";
    else cmd += "x ";

    if ( !pck ) cmd += file;
    else if ( key.empty() ) cmd += file + ".zpaq " + file;
    else cmd += file + ".zpc " + file;

    cmd += " -m5";

    if ( !key.empty() ) cmd += " -key " + key;

    ///cout << " " << __func__ << '|' << cmd << '\n';
    std::cout << std::flush; std::cerr << std::flush;
    int rsys = std::system(cmd.c_str());
    return rsys;
}

int gfu::cmix(string file, bool pck)
{
    {
        size_t sz = 32; // Gb
        sz *= 1024u * 1024 * 1024;
        char * p = new char[sz];
        if ( !p ) throw "not enough memory for cmix";
        for_i(10) p[i] = 'a';
        delete []p;
    }

    string cmd = "cmix ";
    if ( pck ) cmd += "-c ";
    else cmd += "-d ";


    string fnout = file + ".cx";
    if ( pck ) cmd += file + " " + fnout;
    else
    {
        if ( !ol::endsWith(file, ".cx", fnout) )
            throw "file [" + file + "] is not .cx";
        cmd += file + " " + fnout;
    }

    std::cout << std::flush; std::cerr << std::flush;
    int rsys = std::system(cmd.c_str());

    if ( !fs::exists(fnout) ) throw "cmix failed to make file";

    if ( fs::file_size(fnout) == 0 )
    {
        fs::remove(fnout);
        throw "cmix failed to run";
    }

    return rsys;
}

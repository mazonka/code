#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
//#include <format>

using std::string;
namespace fs = std::filesystem;
using std::cout;

int g_depth;
unsigned long long g_lftime;
string g_av1;

namespace hash
{
string hashHex(const string & s);
string toHex(const string & s);
string toBin(const string & s);
}

int find_depth()
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
}

fs::path find_key()
{
    auto cwd = fs::current_path();

    fs::path r;

    for ( int i = g_depth; i >= 0; i-- )
    {
        auto cp = fs::current_path();
        auto cf = cp / "bzc.key";
        //cout << cf.string() << '\n';
        if ( fs::is_regular_file(cf) ) { r = cf; break; }
        fs::current_path("..");
    }

    fs::current_path(cwd);
    return r;
}

string hexor(string a, string b)
{
    using namespace hash;
    auto ba = toBin(a);
    auto bb = toBin(b);
    if ( ba.size() != 32 || bb.size() != 32 ) throw "Bad hexor size";
    string r; r.resize(32,'\0');
    for ( int i = 0; i < 32; i++) r[i] = a[i] ^ b[i];
    return toHex(r);
}

void make_key()
{
    auto cwd = fs::current_path();

    fs::path kf;

    for ( int i = g_depth; i >= 0; i-- )
    {
        auto cp = fs::current_path();
        auto cf = cp / "bzc.key";
        //cout << cf.string() << '\n';

        {
            std::ofstream of(cf);
            if ( !of ) break;
        }

        fs::remove(cf);

        kf = cf;
        fs::current_path("..");
    }

    fs::current_path(cwd);

    cout << "Do you want to (re)create key file on this device (y/n)? ";
    string s;
    std::cin >> s;
    if ( s != "y" ) { cout << "bye\n"; return; }

    cout << "File to be created: " << kf.string() << '\n';

    cout << "Enter your password: ";
    std::cin >> s;

    cout << "[" << s << "]\n";

    auto t = hash::hashHex(std::to_string(g_lftime));
    auto sh = hash::hashHex(s);
    //sh = hash::hashHex(sh);
    auto sx = hexor(sh, t);

    std::ofstream of(kf, std::ios::binary);
    of << sx << '\n';
    of << hash::hashHex(t) << '\n'; // double hash
}

bool endsWith(string s, string fx)
{
    auto sz = s.size();
    auto fz = fx.size();
    return sz >= fz && 0 == s.compare(sz - fz, fz, fx);
};

template <class T>
inline size_t x2st(T x) { return static_cast<size_t>( x ); }
string file2str(const string & file)
{
    const size_t MAX_FILE_SIZE = 1024u * 1024 * 1000; // 1000Mb
    std::ifstream in(file.c_str(), std::ios::binary);

    if ( !in )
        return "";

    string r;

    in.seekg(0, std::ios::end);

    size_t sz = x2st(in.tellg());

    if ( sz > MAX_FILE_SIZE ) throw "File tto big";

    r.reserve( sz );
    in.seekg(0, std::ios::beg);

    r.assign( std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() );

    return r;
}


void run(string file, string hkey)
{
    if ( file.empty() ) return;

    cout << "Input file:  " << file << '\n';

    string abkey = hash::toBin(hkey);
    if ( abkey.size() != 32 ) throw "Bad key size";
    //cout << "bkey.size " << abkey.size() << '\n';
    //cout << "hkey " << hash::toHex(bkey) << '\n';

    string ofile;

    if (0) {}
    else    if ( endsWith(file, ".bz2") )
        ofile = file.substr(0, file.size() - 4) + ".bzc";
    else    if ( endsWith(file, ".bzc") )
        ofile = file.substr(0, file.size() - 4) + ".bz2";
    else throw "File name is not .bz2 or .bzc";

    cout << "Output file: " << ofile << '\n';

    string sfile = file2str(file);

    cout << "File loaded: " << sfile.size() << '\n';

    auto sz = sfile.size();
    string bkey;
    for ( size_t i = 0; i < sz; i++ )
    {
        if ( !(i % 32) )
        {
            bkey = hash::toBin(hkey);
            hkey = hash::hashHex(hkey);
        }

        sfile[i] = sfile[i] ^ bkey[i % 32];
    }

    std::ofstream of(ofile, std::ios::binary);
    of << sfile;
}

void help()
{
    cout << "Usage: commands: makekey\n";
    throw "Command [" + g_av1 + "] not recognized";
}

int main(int ac, const char * av[])
try
{
    auto ftime = fs::last_write_time(av[0]);
    auto cftime = 1ull * ftime.time_since_epoch().count();
    g_lftime = cftime;
    //g_lftime = 1; // FIXME
    //cout <<  g_lftime << '\n';
    string hash_lftime1 = hash::hashHex(std::to_string(g_lftime));
    string hash_lftime2 = hash::hashHex(hash_lftime1);

    g_depth = find_depth();
    //cout << "depth = " << find_depth() << '\n';

    auto keyf = find_key();

    if ( ac > 1 )
    {
        g_av1 = string(av[1]);
        auto bcmd = (g_av1.find('.') == string::npos );

        if (bcmd)
        {
            if (0) {}
            else if ( g_av1 == "makekey" ) make_key();
            else help();
            return 0;
        }
    }

    if ( keyf.empty() )
    {
        cout << "Key not found, rerun with 'makekey'\n";
        return 0;
    }

    cout << "key file: " << keyf.string() << '\n';

    string key;
    {
        std::ifstream in(keyf);
        string pwd, htime;
        in >> pwd >> htime;

        if ( htime != hash_lftime2 )
        {
            cout << "Key [" << keyf.string() << "] expired. Please regenerate\n";
            return 1;
        }
        key = hash::hashHex(hexor(pwd,hash_lftime1));
    }

    //cout << "key encr: " << key << '\n';

    string file;
    if ( ac > 1 ) file = av[1];

    run(file, key);
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


#include "hash.cpp"

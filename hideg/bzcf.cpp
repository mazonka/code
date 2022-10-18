#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <chrono>

#include "olu.h"
#include "hash.h"

using std::string;
namespace fs = std::filesystem;
using std::cout;

int g_depth;
string g_stime;
string g_av1;
string g_keyfilename;
string g_hkey;

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
        ///auto cf = cp / "bzc.key";
        auto cf = cp / g_keyfilename;
        //cout << cf.string() << '\n';
        if ( fs::is_regular_file(cf) ) { r = cf; break; }
        fs::current_path("..");
    }

    fs::current_path(cwd);
    return r;
}

string hexor(string a, string b)
{
    using namespace ha;
    auto ba = toBin(a);
    auto bb = toBin(b);
    if ( ba.size() != 32 || bb.size() != 32 ) throw "Bad hexor size";
    string r; r.resize(32, '\0');
    for (int i = 0; i < 32; i++)
    {
        unsigned ua = (unsigned char)ba[i];
        unsigned ub = (unsigned char)bb[i];
        r[i] = char(ua ^ ub);
    }
    return toHex(r);
}

void make_key(string file)
{
    auto cwd = fs::current_path();

    fs::path kf = file;

    if ( file.empty() )
    {
        for ( int i = g_depth; i >= 0; i-- )
        {
            auto cp = fs::current_path();
            ///auto cf = cp / "bzc.key";
            auto cf = cp / g_keyfilename;
            //cout << cf.string() << '\n';

            {
                std::ofstream of(cf);
                if ( !of ) break;
            }

            fs::remove(cf);

            kf = cf;
            fs::current_path("..");
        }
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

    auto t1 = ha::hashHex(g_stime);
    auto sh = ha::hashHex(s);
    //cout << "shash : " << sh << '\n';
    //sh = hash::hashHex(sh);
    auto sx = hexor(sh, t1);

    std::ofstream of(kf, std::ios::binary);
    of << sx << '\n';
    of << ha::hashHex(t1) << '\n'; // double hash
}



int run(string file, string hkey, string ofile, int enc, bool chkonly)
// enc - 0 auto, 1 enc, 2 dec
{
    if (file.empty())
    {
        g_hkey = hkey;
        return 0;
    }

    cout << "Input file : " << file << '\n';

    string abkey = ha::toBin(hkey);
    if ( abkey.size() != 32 ) throw "Bad key size";
    //cout << "bkey.size " << abkey.size() << '\n';
    //cout << "hkey " << hash::toHex(bkey) << '\n';

    string file4;
    if ( file.size() > 4 ) file4 = file.substr(0, file.size() - 4);

    if ( enc == 0 )
    {

        if (!ofile.empty()) {}
        else    if ( ol::endsWith(file, ".bz2") )
        {
            ofile = file4 + ".bzc";
            enc = 1;
        }
        else    if ( ol::endsWith(file, ".bzc") )
        {
            ofile = file4 + ".bz2";
            enc = 2;
        }
        else throw "File name is not .bz2 or .bzc";
    }
    else
    {
        if (!ofile.empty()) {}
        else if ( enc == 1 )
        {
            if ( ol::endsWith(file, ".bz2") )
                ofile = file4 + ".bzc";
            else ofile = file + ".gfc";
        }
        else if ( enc == 2 )
        {
            if ( ol::endsWith(file, ".bzc") )
                ofile = file4 + ".bz2";
            else if ( ol::endsWith(file, ".gfc") )
                ofile = file4;
            else "File name is not .gfc or .bzc";
        }
        else never;
    }

    cout << "Output file: " << ofile << '\n';

    string sfile = ol::file2str(file);

    cout << "File loaded: " << sfile.size() << '\n';

    auto sz = sfile.size();

    string salt, chksum;
    if ( enc == 0 ) throw "Bad enc type";
    else if ( enc == 1 )
    {
        chksum = ha::hashHex(sfile);
        chksum = ha::toBin(chksum);
        chksum = chksum.substr(0, 4);
        auto isalt = std::chrono::steady_clock::now().time_since_epoch().count();
        salt = std::to_string(isalt);
        salt = ha::hashHex(salt);
        salt = ha::toBin(salt);
        salt = salt.substr(0, 4);
    }
    else if ( enc == 2 )
    {
        if ( sz < 8 ) throw "Bad file";
        chksum = sfile.substr(sz - 4);
        salt = sfile.substr(sz - 8, 4);
        sz -= 8;
        sfile.resize(sz);
    }

    string bkey;
    for ( size_t i = 0; i < sz; i++ )
    {
        if ( !(i % 32) )
        {
            hkey = ha::hashHex(salt + hkey);
            bkey = ha::toBin(hkey);
        }

        sfile[i] = sfile[i] ^ bkey[i % 32];
    }

    if ( enc == 2 )
    {
        auto chk = ha::hashHex(sfile);
        chk = ha::toBin(chk);
        chk = chk.substr(0, 4);
        if ( chk != chksum )
        {
            cout << "\nCheck sum FAILED\n";
            return 1;
        }
    }

    if ( chkonly )
    {
        cout << "\nCheck sum OK\n";
        return 0;
    }

    std::ofstream of(ofile, std::ios::binary);
    of << sfile;

    if ( enc == 1 ) of << salt << chksum;

    return 0;
}

void die()
{
    cout << "bzc, ver 1.0.1, Oleg Mazonka 2022\n";
    cout << "Usage:\n";
    cout << "\t: file.bz2 [file_out]\n";
    cout << "\t: file.bzc [file_out]\n";
    cout << "\tcommands: genkey [file], {enc|dec} file_in [file_out], chk file\n";
    throw "Command [" + g_av1 + "] not recognized";
}

int main_bzc(string arg0, ivec<string> args1)
{
    ivec<string> args;
    args += arg0;
    args += args1;
    int ac = args.size();
    auto & avs = args;

    {
        auto exe = fs::path(avs[0]);
        g_keyfilename = "." + exe.stem().string() + ".key";
        auto ftime = fs::last_write_time(exe);
        auto cftime = 1ull * ftime.time_since_epoch().count();
        g_stime = std::to_string((unsigned long long)cftime);
    }
    //cout <<  g_lftime << '\n';
    string hash_stime1 = ha::hashHex(g_stime);
    string hash_stime2 = ha::hashHex(hash_stime1);

    //cout << "stime : " << hash_stime1 << '\n';

    g_depth = find_depth();
    //cout << "depth = " << find_depth() << '\n';

    auto keyf = find_key();

    string ifile, ofile;
    int enc = 0;
    bool chkonly = false;

    if ( ac > 1 )
    {
        g_av1 = avs[1];
        auto bcmd = (g_av1.find('.') == string::npos );

        if (bcmd)
        {
            if (0) {}
            else if (g_av1 == "genkey")
            {
                string f;
                if ( ac > 2 ) f = avs[2];
                make_key(f); return 0;
            }
            else if ( g_av1 == "dec" || g_av1 == "enc" )
            {
                if (ac > 2) ifile = avs[2];
                if (ac > 3) ofile = avs[3];
                enc = 1;
                if ( g_av1 == "dec" ) enc = 2;
            }
            else if (g_av1 == "chk")
            {
                if (ac > 2) ifile = avs[2];
                ofile = "<dummy>";
                enc = 2;
                chkonly = true;
            }
            else die();
        }
        else
        {
            ifile = avs[1];
            if ( ac > 2 ) ofile = avs[2];
        }

    }

    if ( keyf.empty() )
    {
        cout << "Key not found, rerun with 'genkey'\n";
        return 0;
    }

    cout << "key file: " << keyf.string() << '\n';

    string key;
    {
        std::ifstream in(keyf);
        string pwd, htime;
        in >> pwd >> htime;

        if ( htime != hash_stime2 )
        {
            cout << "Key [" << keyf.string() << "] expired; use genkey\n";
            return 1;
        }
        key = ha::hashHex(hexor(pwd, hash_stime1));
    }

    //cout << "run key : " << key << '\n';

    return run(ifile, key, ofile, enc, chkonly);
}


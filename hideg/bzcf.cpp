#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <chrono>

#include "olu.h"
#include "hash.h"
#include "gfu.h"

using std::string;
namespace fs = std::filesystem;
using std::cout;

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
        for ( int i = g::depth; i >= 0; i-- )
        {
            auto cp = fs::current_path();
            auto cf = cp / g::keyfilename;
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
    string s; std::cin >> s;
    if ( s != "y" ) { cout << "bye\n"; return; }

    {
        bool x = fs::exists(kf);
        cout << "File to be " << (x ? "updated" : "created") << ": " << kf.string() << '\n';
    }

    cout << "Do you want to proceed (y/n)? ";
    s = ""; std::cin >> s;
    if ( s != "y" ) { cout << "bye\n"; return; }

    cout << "Enter your gf password: ";
    std::cin >> s;

    cout << "[" << s << "]\n";

    auto t1 = ha::hashHex(g::sysuid);
    auto sh = ha::hashHex(s);
    //cout << "shash : " << sh << '\n';
    //sh = hash::hashHex(sh);
    auto sx = hexor(sh, t1);

    std::ofstream of(kf, std::ios::binary);
    of << sx << '\n';
    of << ha::hashHex(t1) << '\n'; // double hash
}



int run_bzcf(string file, string ofile, int enc, bool chkonly)
// enc - 0 auto, 1 enc, 2 dec
{
    const bool P = !g::recursive_mode && !g::silent;

    if (file.empty()) return 0;

    auto hkey = g::hkey;

    if (P) cout << "[ bzc " << (enc == 0 ? "auto" : enc == 1 ? "enc" : "dec") << " ]\n";
    if (P) cout << "Input file : " << file << '\n';

    string abkey = ha::toBin(hkey);
    if ( abkey.size() != 32 ) throw "Bad key size";
    //cout << "bkey.size " << abkey.size() << '\n';
    //cout << "hkey " << hash::toHex(bkey) << '\n';

    string file4;
    if ( enc == 0 )
    {
        if (!ofile.empty()) {}
        else    if ( ol::endsWith(file, ".bz2", file4) )
        {
            ofile = file4 + ".bzc";
            enc = 1;
        }
        else    if ( ol::endsWith(file, ".bzc", file4) )
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
            if ( ol::endsWith(file, ".bz2", file4) )
                ofile = file4 + ".bzc";
            else ofile = file + ".gfc";
        }
        else if ( enc == 2 )
        {
            if ( ol::endsWith(file, ".bzc", file4) )
                ofile = file4 + ".bz2";
            else if ( ol::endsWith(file, ".gfc", file4) )
                ofile = file4;
            else "File name is not .gfc or .bzc";
        }
        else never;
    }

    if (P) cout << "Output file: " << ofile << '\n';

    string sfile = ol::file2str(file);

    if (P) cout << "File loaded: " << sfile.size() << '\n';

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

    {
        std::ofstream of(ofile, std::ios::binary);
        of << sfile;

        if ( enc == 1 ) of << salt << chksum;
    }

    if ( g::keepfile == 2 ) fs::remove(file);

    return 0;
}

void die(string av1)
{
    cout << "bzc gf's module, ver 1.0.3, Oleg Mazonka 2022\n";
    cout << "Usage:\n";
    cout << "\t: file.bz2 [file_out]\n";
    cout << "\t: file.bzc [file_out]\n";
    cout << "\tcommands: genkey [file], {enc|dec} file_in [file_out], chk file\n";
    throw "Command [" + av1 + "] not recognized";
}

int main_bzc(ivec<string> args1)
{
    ivec<string> args;
    args += "";
    args += args1;
    int ac = args.size();
    auto & avs = args;

    //cout <<  g::lftime << '\n';
    string hash_stime1 = ha::hashHex(g::sysuid);
    string hash_stime2 = ha::hashHex(hash_stime1);

    //cout << "stime : " << hash_stime1 << '\n';

    gfu::find_depth();
    //cout << "depth = " << find_depth() << '\n';

    string ifile, ofile;
    int enc = 0;
    bool chkonly = false;

    if ( ac > 1 )
    {
        string av1 = avs[1];
        auto bcmd = (av1.find('.') == string::npos );

        if (bcmd)
        {
            if (0) {}
            else if (av1 == "genkey")
            {
                string f;
                if ( ac > 2 ) f = avs[2];
                make_key(f); return 0;
            }
            else if ( av1 == "dec" || av1 == "enc" )
            {
                if (ac > 2) ifile = avs[2];
                if (ac > 3) ofile = avs[3];
                enc = 1;
                if ( av1 == "dec" ) enc = 2;
            }
            else if (av1 == "chk")
            {
                if (ac > 2) ifile = avs[2];
                ofile = "<dummy>";
                enc = 2;
                chkonly = true;
            }
            else die(av1);
        }
        else
        {
            ifile = avs[1];
            if ( ac > 2 ) ofile = avs[2];
        }

    }

    // genkey must be before key check
    if (g::keyfile.empty())
    {
        gfu::find_key();

        if (g::keyfile.empty())
        {
            cout << "Key not found, rerun with 'genkey'\n";
            return 1;
        }
        //cout << "key file: " << keyf.string() << '\n';
    }


    string key = g::hkey;

    if ( key.empty() )
    {
        std::ifstream in(g::keyfile);
        string pwd, htime;
        in >> pwd >> htime;

        if (ISOPEN)
        {
            cout << "key debug data\n"
                 << "  keyfile " << g::keyfile << "\n"
                 << "  htime   " << htime << "\n"
                 << "  h_st2   " << hash_stime2 << '\n'
                 << "  h_st1   " << hash_stime1 << '\n'
                 << "  sysiud  " << g::sysuid << '\n'
                 << "  gfexe   " << g::gfexe << '\n';
        }

        if ( htime != hash_stime2 )
        {
            cout << "Key [" << g::keyfile.string() << "] expired; use genkey\n";
            return 2;
        }
        key = ha::hashHex(hexor(pwd, hash_stime1));
        g::hkey = key;
    }

    //cout << "run key : " << key << '\n';

    return run_bzcf(ifile, ofile, enc, chkonly);
}


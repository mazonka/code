#include "crun.h"

const bool CHK_CMIX = !true;
const bool CHK_ZPAQ = !!true;

#include <thread>
#include <chrono>
using namespace std::chrono_literals;

#include "../hash.h"
#include "../olu.h"



#define tsys(s) if( sys(s)){ nevers(s); }
#define fail nevers("FAILED");

void esys2(string s, string o)
{
    sys(s + " > " + o);
    auto f = ol::file2str(o);
    ol::replaceAll(f, "error", "throw");
    cout << f;
    fs::remove(o);
}

void esys(string s)
{
    esys2(s, "tXX.out");
}

void save(fs::path ft, string s)
{
    std::this_thread::sleep_for(10ms);
    ofstream of(ft);
    of << s;
    if (!of) never;
}

string gf = (fs::path("..") / ".." / "gf.exe").string() + " ";
string gfk = (fs::path("..") / ".." / "gf.exe -k").string() + " ";
string gfd = (fs::path("..") / ".." / "gf.exe -d").string() + " ";

struct Subext
{
    string ext;
    string pck, unp;
    int keep; // 0 - del; 1 - keep
};

struct Ext
{
    string ext;
    string pck, unp;
    int defkeep; // 0 - def del; 1 - def keep
    string pck_k, unp_k;
    string pck_d, unp_d;
};

void cmain14()
{
    cout << "pack/unpack/co/sync different exts:";
    ivec<string> valid_exts
    {
        {".bz2", ".g", ".gfc", ".bzc", ".fcl", ".cx", ".zpaq", ".zpc"}
    };

    ivec<Ext> valid_sexts
    {
        {
            {
                ".zpc", gf + "zpaq", gf + "unpack", 1,
                gfk + "zpaq", gfk + "unpack", gfd + "zpaq", gfd + "unpack"
            },
            {
                ".zpaq", "zpaq a $.zpaq $ -m5", gf + "unpack", 1,
                "zpaq a $.zpaq $ -m5", gfk + "unpack", "zpaq a $.zpaq $ -m5 && rm $", gfd + "unpack"
            },
            {
                ".cx", gf + "cmix", gf + "unpack", 1,
                gfk + "cmix", gfk + "unpack", gfd + "cmix", gfd + "unpack"
            },
            {
                ".bz2", "bzip2", "bzip2 -d", 0,
                "bzip2 -k", "bzip2 -k -d", "bzip2", "bzip2 -d"
            },
            {
                ".g", gf + "g", gf + "g", 1,
                gfk + "g", gfk + "g", gfd + "g", gfd + "g"
            },
            {
                ".gfc", gf + "bzc enc", gf + "bzc dec", 1,
                gfk + "bzc enc", gfk + "bzc dec", gfd + "bzc enc", gfd + "bzc dec"
            },
            {
                ".bzc", gf + "pack", gf + "unpack", 0,
                gfk + "pack", gfk + "unpack", gfd + "pack", gfd + "unpack"
            }
        }
    };

    auto all_exts = valid_exts; all_exts.clear();
    for ( auto x : valid_exts )
    {
        if ( !CHK_CMIX && x == ".cx" ) continue;
        if ( !CHK_ZPAQ && x == ".zpc" ) continue;
        if ( !CHK_ZPAQ && x == ".zpaq" ) continue;
        all_exts += x;
    }

    cout << all_exts.add(" ") << '\n';

    auto all_sexts = valid_sexts; all_sexts.clear();
    for ( auto x : valid_sexts )
    {
        if ( !CHK_CMIX && x.ext == ".cx" ) continue;
        if ( !CHK_ZPAQ && x.ext == ".zpc" ) continue;
        if ( !CHK_ZPAQ && x.ext == ".zpaq" ) continue;
        all_sexts += x;
    }

    fs::create_directory("t14re");
    fs::create_directory("t14co");

    auto pck = [](Subext e, bool pkonly = false)
    {
        string fn = "a_" + e.ext.substr(1) + ".txt";
        save(fn, "123");
        auto cmd_pck = (e.pck + " " + fn);

        if ( e.pck.find("$") != string::npos )
        {
            cmd_pck = e.pck;
            ol::replaceAll(cmd_pck, "$", fn);
        }

        cout << "# " << cmd_pck << '\n';
        tsys(cmd_pck);
        auto fnp = fn + e.ext;
        if ( !fs::exists(fnp) ) fail;
        if ( e.keep )
        {
            if ( !fs::exists(fn) ) fail;
            fs::remove(fn);
        }
        else if ( fs::exists(fn) ) fail;

        if ( ol::file2str(fn) != "" ) fail;

        if ( pkonly ) return;

        auto cmd_unp = (e.unp + " " + fnp);
        cout << "# " << cmd_unp << '\n';
        tsys(cmd_unp);
        if ( !fs::exists(fn) ) fail;
        if ( e.keep )
        {
            if ( !fs::exists(fnp) ) fail;
            fs::remove(fnp);
        }
        else if ( fs::exists(fnp) ) fail;
        if ( ol::file2str(fn) != "123" ) fail;
    };

    // in t14re
    {
        ol::Pushd pd("t14re");
        for ( auto e : all_sexts)
        {
            pck({e.ext, e.pck, e.unp, e.defkeep});
            pck({e.ext, e.pck_k, e.unp_k, 1});
            pck({e.ext, e.pck_d, e.unp_d, 0});
        }

        for ( auto e : all_sexts) pck({e.ext, e.pck_d, e.unp_d, 0}, true);
    }

    auto syn = [](Ext e)
    {
        string fn = "a_" + e.ext.substr(1) + ".txt";

        // check c.txt content
        if ( ol::file2str(fn) != "123" ) fail;

        // update c content
        save(fn, "45");

        // sync
        tsys(gf + "sync");

        // remove c
        fs::remove(fn);

        // check c no exists
        if ( ol::file2str(fn) != "" ) fail;

        // sync
        tsys(gf + "sync");

        // check c content
        if ( ol::file2str(fn) != "45" ) fail;
    };

    {
        ol::Pushd pd("t14co");
        // in t14co make co

        tsys(gf + "co ../t14re");
        for ( auto e : all_sexts) syn(e);
    }

    // cleanup
    fs::remove_all("t14re");
    fs::remove_all("t14co");

    cout << "t14 14 OK\n";
}


void cmain15()
{
    cout << "pack/unpack/co/sync .fcl\n";

    Ext sext
    {
        ".fcl", gf + "fcl make", gf + "fcl extr", 1,
        gfk + "fcl make", gfk + "fcl extr", gfd + "fcl make", gfd + "fcl extr"
    };

    fs::create_directory("t15re");
    fs::create_directory("t15co");

    auto pck = [](Subext e, bool pkonly = false)
    {
        string dn = "a_" + e.ext.substr(1);
        fs::create_directory(dn);
        string fn = dn + "/a.txt";
        save(fn, "123");
        auto cmd_pck = (e.pck + " " + dn);

        cout << "# " << cmd_pck << '\n';
        tsys(cmd_pck);
        auto fnp = dn + e.ext;
        if ( !fs::exists(fnp) ) fail;
        if ( e.keep )
        {
            if ( !fs::exists(dn) ) fail;
            fs::remove_all(dn);
        }
        else if ( fs::exists(dn) ) fail;

        if ( ol::file2str(fn) != "" ) fail;

        if ( pkonly ) return;

        auto cmd_unp = (e.unp + " " + fnp);
        cout << "# " << cmd_unp << '\n';
        tsys(cmd_unp);
        if ( !fs::exists(fn) ) fail;
        if ( e.keep )
        {
            if ( !fs::exists(fnp) ) fail;
            fs::remove(fnp);
        }
        else if ( fs::exists(fnp) ) fail;
        if ( ol::file2str(fn) != "123" ) fail;

		fs::remove_all(dn);
    };

    // in t15re
    {
        ol::Pushd pd("t15re");

        auto e = sext;
        pck({e.ext, e.pck, e.unp, e.defkeep});
        pck({e.ext, e.pck_k, e.unp_k, 1});
        pck({e.ext, e.pck_d, e.unp_d, 0});
        pck({e.ext, e.pck_d, e.unp_d, 0}, true);
    }

    auto syn = [](Ext e)
    {
        tsys(gf + "st");

        string dn = "a_" + e.ext.substr(1);
        string fn = dn + "/a.txt";

        // check c.txt content
        if ( ol::file2str(fn) != "123" ) fail;

        // update c content
        save(fn, "45");

        // sync
        tsys(gf + "sync");

        // remove c
        fs::remove_all(dn);

        // check c no exists
        if ( ol::file2str(fn) != "" ) fail;

        // sync
        tsys(gf + "sync");

        // check c content
        if ( ol::file2str(fn) != "45" ) fail;
    };

    {
        ol::Pushd pd("t15co");
        // in t15co make co

        tsys(gf + "co ../t15re");
        syn(sext);
    }

    // cleanup
    fs::remove_all("t15re");
    fs::remove_all("t15co");

    cout << "t14 15 OK\n";
}

void cmain16()
{
    cout << "pack/unpack/co/sync chain\n";


    fs::create_directory("t16re");
    fs::create_directory("t16co");

    // in t16re
    {
        ol::Pushd pd("t16re");

		save("a.txt","123");
		tsys(gf+"-d g a.txt");
		tsys(gf+"-d bzc enc a.txt.g");
		tsys(gf+"-d g a.txt.g.gfc");
		tsys(gf+"-d bzc enc a.txt.g.gfc.g");

		save("b.txt","123");
		tsys(gf+"-d bzc enc b.txt");
    }

    {
        ol::Pushd pd("t16co");
        tsys(gf + "co ../t16re");
		save("a.txt","45");
        tsys(gf + "sync");
		save("b.txt","45");
        tsys(gf + "sync");
    }

	// test unpack
    {
        ol::Pushd pd("t16re");

		tsys(gf+"unpack a.txt.g.gfc.g.gfc");
		tsys(gf+"unpack b.txt.gfc");

		if( ol::file2str("a.txt") != "45" ) fail;
		if( ol::file2str("b.txt") != "45" ) fail;
    }

    // cleanup
    fs::remove_all("t16re");
    fs::remove_all("t16co");

    cout << "t14 16 OK\n";
}

void cmain()
{
    cmain14();
    cmain15(); // fcl
    cmain16(); // chain exts
}

#include "../hash.cpp"
#include "../sha2.cpp"
#include "../olu.cpp"

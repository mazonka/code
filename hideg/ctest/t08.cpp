#include "crun.h"

#include <thread>
#include <chrono>
using namespace std::chrono_literals;

#include "../hash.h"
#include "../olu.h"


//void tsys(string s) { if( sys(s)) nevers(s); }
#define tsys(s) if( sys(s)){ nevers(s); }

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
    esys2(s, "t08.out");
}

void save(fs::path ft, string s)
{
    std::this_thread::sleep_for(10ms);
    ofstream of(ft);
    of << s;
    if (!of) never;
}

void cmain()
{

    string a; ol::endsWith("abcdef", "ef", a); cout << a << '\n';
    string gf = (fs::path("..") / "gf.exe ").string();
    string gf2 = (fs::path("..") / ".." / "gf.exe ").string();

    esys( gf + "sync @"); // no .gf - error - forced
    esys( gf + "sync @."); // no .gf - error - forced
    esys( gf + "sync @.."); // no .gf - error - forced
    tsys( gf + "sync"); // no .gf - ok - recursive
    tsys( gf + "sync ."); // no .gf - ok - recursive
    tsys( gf + "sync .."); // no .gf - ok - recursive
    esys( gf + "co aaa"); // no file
    esys( gf + "sync makefile"); // no entry
    esys( gf + "co makefile"); // dst==src
    esys( gf + "co ../makefile"); // file exists

    tsys( gf + "co ../gf.exe"); // ok
    esys( gf + "co ../gf.exe"); // entry exists
    fs::remove_all(".gf"); // cleanup
    fs::remove("gf.exe");

    // test1 - simple
    if (1)
    {
        fs::path ds = "t08s";
        fs::path dd = "t08d";
        string ft = "t08.txt";
        string dsft = (ds / ft).string();
        fs::create_directory(ds);
        fs::create_directory(dd);
        ofstream(ds / ft) << "123";
        tsys( gf + "pack " + dsft ); // ok
        {
            ol::Pushd pushd(dd);
            tsys( gf2 + "co ../" + dsft + ".bzc"); // ok
            ///if ( ol::file2str(ft) != "123" ) nevers("FAILED");
            auto body = ol::file2str(ft);
            if ( body != "123" ) nevers("FAILED [" + body + "]");
            tsys( gf2 + "sync"); // no action
            tsys( gf2 + "st"); // nothing
            tsys( gf2 + "st @"); // [I]
            tsys( gf2 + "st " + ft); // [I]
        }

        ofstream(ds / ft) << "1234";
        tsys( gf + "pack " + dsft ); // ok
        {
            ol::Pushd pushd(dd);
            tsys( gf2 + "st"); // [L]
            tsys( gf2 + "sync"); // L-update
            if ( ol::file2str(ft) != "1234" ) nevers("FAILED");
            save(ft, "12345");
            tsys( gf2 + "st " + ft); // [M]
            tsys( gf2 + "st"); // [M]
            tsys( gf2 + "sync"); // M-update
            fs::remove(ft);
            tsys( gf2 + "sync"); // A-update
            auto bod = ol::file2str(ft);
            if ( bod != "12345" ) nevers("FAILED [" + bod + "]");
        }

        fs::remove_all(".gf");
        fs::remove_all(ds);
        fs::remove_all(dd);
    }


    // test2 - recursive
    if (2)
    {
        string ds0_s = "t08s";
        fs::path ds0 {ds0_s};
        fs::path ab {"a"}; ab = ab / "b";
        auto ds = ds0 / "a" / "b";
        fs::path dd = "t08d";
        string ft = "t08.txt";
        string dsft = (ds / ft).string();
        fs::create_directories(ds);
        fs::create_directory(dd);
        ofstream(ds / ft) << "123";
        tsys( gf + "pack " + dsft ); // ok
        {
            ol::Pushd pushd1(dd);
            { ofstream("a") << ""; }
            esys( gf2 + "co ../" + ds0_s); // not empty
            fs::remove("a");
            fs::path upds(".."); upds = upds / ds0;
            tsys( gf2 + "co " + upds.string() ); // ok
            {
                ol::Pushd pd2("a/b");
                if ( string fs; (fs = ol::file2str(ft)) != "123" )
                    nevers("FAILED [" + fs + "]");
            }
            tsys( gf2 + "sync"); // no action
            tsys( gf2 + "st"); // nothing
            tsys( gf2 + "st @"); // [I]
            tsys( gf2 + "st a/b/" + ft); // [I]
        }

        ofstream(ds / ft) << "1234";
        tsys( gf + "pack " + dsft ); // ok
        {
            ol::Pushd pushd2(dd);
            tsys( gf2 + "st"); // [L]
            tsys( gf2 + "sync"); // L-update
            {
                ol::Pushd pd2("a/b");
                if ( string fs; (fs = ol::file2str(ft)) != "1234" )
                    nevers("FAILED [" + fs + "]");
            }

            ///{ ofstream of(ab / ft); of << "12345"; if (!of) never; }
            save(ab / ft, "12345");
            tsys( gf2 + "st"); // [M]
            tsys( gf2 + "sync"); // M-update
            fs::remove(ab / ft);
            tsys( gf2 + "sync"); // A-update
            auto body = ol::file2str(ab / ft);
            if ( body != "12345" ) nevers("FAILED [" + body + "]");
        }

        fs::remove_all(".gf");
        fs::remove_all(ds0);
        fs::remove_all(dd);
    }

    if (3)
    {
        cout << "TEST 3: exts: .gfc .bzc .g .fcl .bz2\n";
        fs::path ds = "t08s";
        fs::path dd = "t08d";
        string ft1 = "t08_bzc.txt";
        string ft2 = "t08_g.txt";
        string dsft1 = (ds / ft1).string();
        string dsft2 = (ds / ft2).string();
        fs::create_directory(ds);
        fs::create_directory(dd);
        ofstream(ds / ft1) << "123";
        ofstream(ds / ft2) << "123";

        tsys( gf + "pack " + dsft1 ); // ok
        tsys( gf + "g " + dsft2 ); // ok

        auto cmd_co = gf2 + "co ../" + ds.string();
        {
            ol::Pushd pushd(dd);
            esys2(cmd_co, "../t08.out"); // fail: conflicing files in src
        }

        fs::remove(dsft2);

        {
            ol::Pushd pushd(dd);
            tsys( cmd_co ); // ok
            auto body1 = ol::file2str(ft1);
            auto body2 = ol::file2str(ft2);
            if ( body1 != "123" ) nevers("FAILED [" + body1 + "]");
            if ( body2 != "123" ) nevers("FAILED [" + body2 + "]");
            tsys( gf2 + "st @"); // [I]
        }

        save(ds / ft1, "1234");
        save(ds / ft2, "1234");

        tsys( gf + "pack " + dsft1 ); // ok
        tsys( gf + "g " + dsft2 ); // ok
        fs::remove(dsft2); // FIXME use -d

        if ( fs::exists(dsft2) ) nevers("FAILED");

        {
            ol::Pushd pushd(dd);
            tsys( gf2 + "st"); // [L]
            tsys( gf2 + "sync"); // L-update
            if ( ol::file2str(ft1) != "1234" ) nevers("FAILED");
            if ( ol::file2str(ft2) != "1234" ) nevers("FAILED");

            save(ft1, "12345");
            save(ft2, "12345");

            tsys( gf2 + "st"); // [M]
            tsys( gf2 + "sync"); // M-update
            fs::remove(ft1);
            fs::remove(ft2);
            tsys( gf2 + "sync"); // A-update
            auto bod1 = ol::file2str(ft1);
            if ( bod1 != "12345" ) nevers("FAILED [" + bod1 + "]");
            auto bod2 = ol::file2str(ft2);
            if ( bod2 != "12345" ) nevers("FAILED [" + bod2 + "]");
        }

        fs::remove_all(".gf");
        fs::remove_all(ds);
        fs::remove_all(dd);
    }

    // test4 - clean
    if (0)
    {
        string ds0_s = "t08s";
        fs::path ds0 {ds0_s};
        fs::path a {"a"};
        auto ds = ds0 / a;
        fs::path dd = "t08d";
        string ft1 = "t08_1.txt";
        string ft2 = "t08_2.txt";
        string dsft1 = (ds / ft1).string();
        string dsft2 = (ds / ft2).string();
        fs::create_directories(ds);
        fs::create_directory(dd);
        ofstream(ds / ft1) << "1-123";
        ofstream(ds / ft2) << "2-123";

        tsys( gf + "pack " + dsft1 ); // ok
        tsys( gf + "pack " + dsft2 ); // ok
        {
            ol::Pushd pushd1(dd);
            fs::path upds(".."); upds = upds / ds0;
            tsys( gf2 + "co " + upds.string() ); // ok

            tsys( gf2 + "clean " + (a / ft1).string());
        }

        return;
        fs::remove_all(".gf");
        fs::remove_all(ds0);
        fs::remove_all(dd);
    }

    cout << "t08 OK\n";
}


#include "../hash.cpp"
#include "../sha2.cpp"
#include "../olu.cpp"

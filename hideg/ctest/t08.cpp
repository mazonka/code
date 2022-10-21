#include "crun.h"

#include "../hash.h"
#include "../olu.h"


//void tsys(string s) { if( sys(s)) nevers(s); }
#define tsys(s) if( sys(s)){ nevers(s); }

void esys(string s)
{
    string o = "t08.out";
    sys(s + " > " + o);
    auto f = ol::file2str(o);
    ol::replaceAll(f, "error", "throw");
    cout << f;
    fs::remove(o);
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

    // test1
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
            if ( ol::file2str(ft) != "123" ) throw "FAILED";
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
            if ( ol::file2str(ft) != "1234" ) throw "FAILED";

            ofstream(ft) << "12345";
            tsys( gf2 + "st"); // [M]
            tsys( gf2 + "sync"); // M-update
            fs::remove(ft);
            tsys( gf2 + "sync"); // A-update
            if ( ol::file2str(ft) != "12345" ) throw "FAILED";
        }

        fs::remove_all(".gf");
        fs::remove_all(ds);
        fs::remove_all(dd);
    }


    // test2
    if (1)
    {
        string ds0_s = "t08s";
        fs::path ds0 {ds0_s};
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
                    throw "FAILED [" + fs + "]";
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
            return;
            tsys( gf2 + "sync"); // L-update
            if ( ol::file2str(ft) != "1234" ) throw "FAILED";

            ofstream(ft) << "12345";
            tsys( gf2 + "st"); // [M]
            tsys( gf2 + "sync"); // M-update
            fs::remove(ft);
            tsys( gf2 + "sync"); // A-update
            if ( ol::file2str(ft) != "12345" ) throw "FAILED";
        }

        fs::remove_all(".gf");
        fs::remove_all(ds);
        fs::remove_all(dd);
    }

    cout << "t08 OK\n";
}


#include "../hash.cpp"
#include "../sha2.cpp"
#include "../olu.cpp"

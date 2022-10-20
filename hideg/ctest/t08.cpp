#include "crun.h"

#include "../hash.h"
#include "../olu.h"


//void tsys(string s) { if( sys(s)) nevers(s); }
#define tsys(s) if( sys(s)){ nevers(s); }

void fsys(string s)
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

    fsys( gf + "sync @"); // no .gf - error - forced
    fsys( gf + "sync @."); // no .gf - error - forced
    fsys( gf + "sync @.."); // no .gf - error - forced
    tsys( gf + "sync"); // no .gf - ok - recursive
    tsys( gf + "sync ."); // no .gf - ok - recursive
    tsys( gf + "sync .."); // no .gf - ok - recursive
    fsys( gf + "co aaa"); // no file
    fsys( gf + "sync makefile"); // no entry
    fsys( gf + "co makefile"); // dst==src
    fsys( gf + "co ../makefile"); // file exists

    tsys( gf + "co ../gf.exe"); // ok
    fsys( gf + "co ../gf.exe"); // entry exists
    fs::remove_all(".gf"); // cleanup
    fs::remove("gf.exe");

    //tsys( gf + "co ../hello.g"); // ok

    //fsys( gf + "co .."); // error - cwd not empty

    cout<<"t08 OK\n";
}


#include "../hash.cpp"
#include "../sha2.cpp"
#include "../olu.cpp"

#include "crun.h"

#include "../hash.h"
#include "../olu.h"


//void tsys(string s) { if( sys(s)) nevers(s); }
#define tsys(s) if( sys(s)){ nevers(s); }

void cmain()
{
    fs::remove_all(".gf"); // cleanup

    string a; ol::endsWith("abcdef", "ef", a); cout << a << '\n';
    string gf = (fs::path("..") / "gf.exe ").string();

    //=sys( gf + "sync @"); // no .gf - error - forced
    //=sys( gf + "sync @."); // no .gf - error - forced
    //=sys( gf + "sync @.."); // no .gf - error - forced
    tsys( gf + "sync"); // no .gf - ok - recursive
    tsys( gf + "sync ."); // no .gf - ok - recursive
    tsys( gf + "sync .."); // no .gf - ok - recursive
    //=tsys( gf + "co aaa"); // no file

    //=tsys( gf + "sync makefile"); // dst==src
    //=tsys( gf + "co makefile"); // dst==src
    tsys( gf + "co ../makefile"); // file exists
    //tsys( gf + "co ../gf.exe"); // ok
    //tsys( gf + "co ../gf.exe"); // entry exists

}


#include "../hash.cpp"
#include "../sha2.cpp"
#include "../olu.cpp"

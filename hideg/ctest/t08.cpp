#include "crun.h"

#include "../hash.h"
#include "../olu.h"


//void tsys(string s) { if( sys(s)) nevers(s); }
#define tsys(s) if( sys(s)){ nevers(s); }

void cmain()
{
    string a; ol::endsWith("abcdef", "ef", a); cout << a << '\n';
    string gf = (fs::path("..") / "gf.exe ").string();

    //sys( gf + "sync"); // no .gf

    //tsys( gf + "co makefile"); // dst==src
    //tsys( gf + "co ../makefile"); // file exists
    //tsys( gf + "co ../gf.exe"); // ok

    //fs::remove_all(".gf");

}


#include "../hash.cpp"
#include "../sha2.cpp"
#include "../olu.cpp"

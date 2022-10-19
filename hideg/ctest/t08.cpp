#include "crun.h"

#include "../hash.h"
#include "../olu.h"

//void tsys(string s) { if( sys(s)) nevers(s); }
#define tsys(s) if( sys(s)){ nevers(s); }

void cmain()
{
    string gf = (fs::path("..") / "gf.exe ").string();
    //tsys( gf + "sync");
    //tsys( gf + "sync ../../hideg");
}

#include "../hash.cpp"
#include "../sha2.cpp"
#include "../olu.cpp"

#include "ccrun.h"

void cmain()
{
    string bzc = (fs::path("..") / "gf.exe ").string();

    if ( sys( bzc + "test") ) throw "FAILED";

    cout << "test3 OK\n";
}

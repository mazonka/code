#include "crun.h"

int sys(string s) { return std::system(s.c_str()); }

void cmain()
{
    string bzc = (fs::path("..") / "gf.exe ").string();

    if ( sys( bzc + "test") ) throw "FAILED";

    cout << "test3 OK\n";
}

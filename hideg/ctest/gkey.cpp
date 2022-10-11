#include "crun.h"

void sys(string s) { std::system(s.c_str()); }

void cmain()
{
    string bzc = (fs::path("..") / "bzc.exe").string();
    if ( !fs::exists("bzc.key") ) sys( bzc + " genkey bzc.key < gkey.txt");
}

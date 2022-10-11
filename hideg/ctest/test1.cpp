#include "crun.h"

void sys(string s) { std::system(s.c_str()); }

void cmain()
{
    {
        ofstream("test1.0.tmp") << "";
    }


    string bzc = (fs::path("..") / "bzc.exe").string();

    if ( !fs::exists("../bzc.key") ) sys( bzc + " genkey ../bzc.key < gkey.txt");

    sys( bzc + " enc test1.0.tmp test1.e.tmp");
    sys( bzc + " dec test1.e.tmp test1.d.tmp");

    if ( fs::exists("test1.d.tmp") ) cout << "OK\n";
    else cout << "FAILED\n";

    ///fs::remove("test1.*.tmp");
    sys("rm -f test1.*.tmp");
}

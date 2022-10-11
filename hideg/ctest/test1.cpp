#include "crun.h"

void sys(string s) { std::system(s.c_str()); }

void cmain()
{
    {
        ofstream("test1.0.tmp") << "";
    }


    string bzc = (fs::path("..") / "bzc.exe").string();

    sys( bzc + " enc test1.0.tmp test1.e.tmp");
    sys( bzc + " dec test1.e.tmp test1.d.tmp");

    if ( !fs::exists("test1.d.tmp") ) throw "FAILED";

    sys("rm -f test1.*.tmp");

    cout << "test1 OK\n";
}

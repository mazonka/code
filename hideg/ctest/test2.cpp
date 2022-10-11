#include "crun.h"

#include "../hash.h"

void sys(string s) { std::system(s.c_str()); }

void cmain()
{
    ostringstream os;
    for ( int i = 0; i < 1000; i++ ) os << i;
    auto s = os.str();
    auto hex = ha::hashHex(s);

    { ofstream("test2.0.tmp") << s; }


    string bzc = (fs::path("..") / "gf.exe bzc ").string();

    sys( bzc + "enc test2.0.tmp test2.e.tmp");
    sys( bzc + "dec test2.e.tmp test2.d.tmp");

    if ( !fs::exists("test2.d.tmp") ) throw "FAILED";

    string s2;
    { ifstream("test2.d.tmp") >> s2; }
    if ( s2 != s ) throw "FAILED";

    sys("rm -f test2.*.tmp");

    cout << "test2 OK\n";

}

#include "../hash.cpp"
#include "../sha2.cpp"

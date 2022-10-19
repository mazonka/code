#include "crun.h"

#include "../hash.h"
#include "../olu.h"

///void sys(string s) { std::system(s.c_str()); }

void cmain()
{
    ostringstream os;
    for ( int i = 0; i < 1000; i++ ) os << i << (i % 12 ? "" : "\n");
    auto s = os.str();
    auto hex = ha::hashHex(s);

    { ofstream("test5.0.tmp", std::ios::binary) << s; }


    string pck = (fs::path("..") / "gf.exe ").string();

    sys( pck + "pack test5.0.tmp");
    sys("rm -f test5.*.tmp");
    sys( pck + "unpack test5.0.tmp.bzc");

    if ( !fs::exists("test5.0.tmp") ) throw "FAILED";

    string s2 = ol::file2str("test5.0.tmp");
    if ( s2 != s ) throw "FAILED";

    sys("rm -f test5.*.tmp");

    cout << "test5 OK\n";

}

#include "../hash.cpp"
#include "../sha2.cpp"
#include "../olu.cpp"

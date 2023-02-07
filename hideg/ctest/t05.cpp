#include "crun.h"

#include "../hash.h"
#include "../olu.h"

///void sys(string s) { std::system(s.c_str()); }
#define tsys(s) if( sys(s)){ nevers(s); }

void cmain()
{
    ostringstream os;
    for ( int i = 0; i < 1000; i++ ) os << i << (i % 12 ? "" : "\n");
    auto s = os.str();
    auto hex = ha::hashHex(s);

    { ofstream("test5.0.tmp", std::ios::binary) << s; }


    string pck = (fs::path("..") / "gf.exe ").string();

    tsys( pck + "pack test5.0.tmp");
    tsys("rm -f test5.*.tmp");
    tsys( pck + "unpack test5.0.tmp.bzc");

    if ( !fs::exists("test5.0.tmp") ) nevers("FAILED");

    string s2 = ol::file2str("test5.0.tmp");
    if ( s2 != s ) throw "FAILED";

    tsys("rm -f test5.*.tmp");

    cout << "t05 OK\n";

}

#include "../hash.cpp"
#include "../sha256.cpp"
#include "../olu.cpp"

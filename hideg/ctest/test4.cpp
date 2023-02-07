#include "crun.h"

#include "../hash.h"
#include "../olu.h"

void cmain()
{
    ostringstream os;
    for ( int i = 0; i < 1000; i++ ) os << i << (i % 12 ? "" : "\n");
    auto s = os.str();
    auto hex = ha::hashHex(s);

    { ofstream("test4.0.tmp", std::ios::binary) << s; }


    string hid = (fs::path("..") / "gf.exe g ").string();

    sys( hid + "test4.0.tmp");
    sys( hid + "test4.0.tmp.g test4.d.tmp");

    if ( !fs::exists("test4.d.tmp") ) throw "FAILED";

    string s2 = ol::file2str("test4.d.tmp");
    if ( s2 != s ) throw "FAILED";

    sys("rm -f test4.*.tmp");
    sys("rm -f test4.*.tmp.g");

    cout << "test4 OK\n";

}

#include "../hash.cpp"
#include "../sha256.cpp"
#include "../olu.cpp"

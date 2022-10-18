#include "crun.h"

#include "../hash.h"
#include "../olu.h"

void sys(string s) { std::system(s.c_str()); }

void cmain()
{
    ostringstream os;
    for ( int i = 0; i < 1000; i++ ) os << i << (i % 12 ? "" : "\n");
    auto s = os.str();
    auto hex = ha::hashHex(s);

    fs::path dir = "test6_dir/a/b";
    fs::create_directories(dir);
    { ofstream(dir / "test6.0.tmp", std::ios::binary) << s; }
    { ofstream(dir / ".." / "test6.1.tmp", std::ios::binary) << s; }


    string fcl = (fs::path("..") / "gf.exe fcl ").string();

    sys( fcl + "make test6_dir");
    //sys("rm -f test6.*.tmp");
    //sys( pck + "unpack test6.0.tmp.bzc");

    //if ( !fs::exists("test6.0.tmp") ) throw "FAILED";

    //string s2 = ol::file2str("test6.0.tmp");
    //if ( s2 != s ) throw "FAILED";

    sys("rm -rf test6_dir");

    //cout << "test6 OK\n";
    cout << "test6 FAILED\n";
}

#include "../hash.cpp"
#include "../sha2.cpp"
#include "../olu.cpp"

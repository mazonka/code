#include "crun.h"

#include "../hash.h"
#include "../olu.h"

///void sys(string s) { std::system(s.c_str()); }
void tsys(string s) { if ( sys(s)) nevers(s); }

void cmain()
{
    ostringstream os;
    for ( int i = 0; i < 1000; i++ ) os << i << (i % 12 ? "" : "\n");
    auto s = os.str();
    auto hex = ha::hashHex(s);


    tsys("rm -rf t06_dir t06_dir.fcl");


    fs::path dir = "t06_dir/a/b";
    fs::create_directories(dir);
    { ofstream(dir / "t06.0.tmp", std::ios::binary) << s; }
    { ofstream(dir / ".." / "t06.1.tmp", std::ios::binary) << s; }


    string fcl = (fs::path("..") / "gf.exe fcl ").string();

    tsys( fcl + "make t06_dir");
    tsys("rm -rf t06_dir");

    tsys( fcl + "extr t06_dir.fcl");

    string s2 = ol::file2str((dir / "t06.0.tmp").string());
    if ( s2 != s ) throw "FAILED";

    tsys("rm -rf t06_dir");

    string s3 = ol::file2str("t06_dir.fcl");
    auto hex3 = ha::hashHex(s3).substr(0, 8);
    ///cout << "hash " << hex3 << '\n';
    if ( hex3 != "804628bb") throw "FAILED hash";

    sys("rm -rf t06_dir t06_dir.fcl");

    cout << "t06 OK\n";
}

#include "../hash.cpp"
#include "../sha256.cpp"
#include "../olu.cpp"

#include "ccrun.h"

#include "../hash.h"
#include "../olu.h"

//void tsys(string s) { if( sys(s)) nevers(s); }
#define tsys(s) if( sys(s)){ nevers(s); }

void cmain()
{
    ostringstream os;
    for ( int i = 0; i < 1000; i++ ) os << i << (i % 12 ? "" : "\n");
    auto s = os.str();
    auto hex = ha::hashHex(s);


    sys("rm -rf t07_dir t07_dir.fcl");


    fs::path dir = "t07_dir/a/b";
    fs::create_directories(dir);
    { ofstream(dir / "t07.0.tmp", std::ios::binary) << s; }
    { ofstream(dir / ".." / "t07.1.tmp", std::ios::binary) << s; }


    string fcl = (fs::path("..") / "gf.exe ").string();

    tsys( fcl + "pack t07_dir");
    tsys("rm -rf t07_dir");

    tsys( fcl + "bzc t07_dir.fcl.bzc");
    string s3 = ol::file2str("t07_dir.fcl.bz2");
    fs::remove("t07_dir.fcl.bz2");
    auto hex3 = ha::hashHex(s3).substr(0, 8);
    if ( hex3 != "fe15d8ea") throw "FAILED hash " + hex3;

    tsys( fcl + "unpack t07_dir.fcl.bzc");

    string s2 = ol::file2str((dir / "t07.0.tmp").string());
    if ( s2 != s ) nevers("FAILED");

    fs::remove_all("t07_dir");


    ///tsys("rm -rf t07_dir t07_dir.fcl.bzc");

    cout << "t07 OK\n";
}

#include "../hash.cpp"
#include "../sha256.cpp"
#include "../olu.cpp"

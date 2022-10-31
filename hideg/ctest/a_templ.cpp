#include "crun.h"

#include <thread>
#include <chrono>
using namespace std::chrono_literals;

#include "../hash.h"
#include "../olu.h"



#define tsys(s) if( sys(s)){ nevers(s); }
#define fail nevers("FAILED");

void esys2(string s, string o)
{
    sys(s + " > " + o);
    auto f = ol::file2str(o);
    ol::replaceAll(f, "error", "throw");
    cout << f;
    fs::remove(o);
}

void esys(string s)
{
    esys2(s, "tXX.out");
}

void save(fs::path ft, string s)
{
    std::this_thread::sleep_for(10ms);
    ofstream of(ft);
    of << s;
    if (!of) never;
}

string gf = (fs::path("..") / "gf.exe ").string();


void cmain()
{
    cout << "test template\n";

    tsys(gf + " info");

    cout << "testX OK\n";
}


#include "../hash.cpp"
#include "../sha2.cpp"
#include "../olu.cpp"


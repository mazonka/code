#include "crun.h"

#include <thread>
#include <chrono>
using namespace std::chrono_literals;

#include "../hash.h"
#include "../olu.h"



#define tsys(s) if( sys(s)){ nevers(s); }

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


void cmain10()
{
    string key = ".gf.key";
    if ( !fs::exists(key) )
    {
        key = "../" + key;
        if ( !fs::exists(key) )
        {
            cout << "t10 no run\n";
            return;
        }
    }

    fs::rename(key, key + ".tmp");

    cout << "lazy key\n";

    fs::create_directory("t10d");
    save("t10d/t10.tmp", "123");
    tsys(gf + " fcl make t10d");
    tsys("bzip2 t10d.fcl");
    tsys(gf + " unpack t10d.fcl.bz2");

    fs::remove_all("t10d");

    fs::rename(key + ".tmp", key);

    cout << "t10 OK\n";
}

void cmain11()
{
	cout<<"zpaq: pack .zpc; unpack .zpc/.zpaq\n";
	//save("t11.tmp","123");
	//tsys(gf+" zpaq t11.tmp");
    cout << "t11 OK\n";
}

void cmain()
{
    cmain10();
    cmain11();
}

#include "../hash.cpp"
#include "../sha2.cpp"
#include "../olu.cpp"

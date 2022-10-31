#include "crun.h"

const bool CHK_CMIX = !true;

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

    cout << "t10 10 OK\n";
}

void cmain11()
{
    cout << "zpaq: pack .zpc; unpack .zpc/.zpaq\n";
    save("t11.tmp", "123");
    tsys(gf + "zpaq t11.tmp");
    tsys("zpaq a t11 t11.tmp");

    fs::remove("t11.tmp");
    string cont = ol::file2str("t11.tmp");
    if ( cont != "" ) fail;
    tsys(gf + "unpack t11.tmp.zpc");

    cont = ol::file2str("t11.tmp");
    if ( cont != "123" ) fail;

    fs::remove("t11.tmp");
    fs::remove("t11.tmp.zpc");
    cont = ol::file2str("t11.tmp");
    if ( cont != "" ) fail;
    tsys(gf + "unpack t11.zpaq");

    cont = ol::file2str("t11.tmp");
    if ( cont != "123" ) fail;

    fs::remove("t11.tmp");

    cout << "t10 11 OK\n";
}

void cmain12()
{
    cout << "zpaq: co/sync\n";
    // create dir t12re t12co
    // make file t12re/a.txt 123; same for b.txt
    // make t12re/a.txt.zpaq
    // make t12re/b.txt.zpc
    // in t12co make co
    // check a.txt b.txt content
    // update a/b content
    // sync
    // remove a/b
    // check a/b no exists
    // sync
    // check a/b content
    // cleanup

    string gf = "..\\..\\gf.exe ";

    // create dir t12re t12co
    fs::create_directory("t12re");
    fs::create_directory("t12co");

    {
        ol::Pushd pd("t12re");
        // make file t12re/a.txt 123; same for b.txt
        save("a.txt", "123");
        save("b.txt", "123");

        // make t12re/a.txt.zpaq
        // make t12re/b.txt.zpc
        tsys("zpaq add a.txt.zpaq a.txt -m5");
        tsys(gf + "zpaq b.txt");
    }

    {
        ol::Pushd pd("t12co");
        // in t12co make co
        esys2(gf + "co ../t12re","../t12.out");
		fs::remove("../t12re/a.txt");
		fs::remove("../t12re/b.txt");
        tsys(gf + "co ../t12re");

        // check a.txt b.txt content
		if( ol::file2str("a.txt" ) != "123" ) fail;
		if( ol::file2str("b.txt" ) != "123" ) fail;

        // update a/b content
        save("a.txt", "a45");
        save("b.txt", "b45");

        // sync
        tsys(gf + "sync");

        // remove a/b
		fs::remove("a.txt");
		fs::remove("b.txt");

        // check a/b no exists
		if( ol::file2str("a.txt" ) != "" ) fail;
		if( ol::file2str("b.txt" ) != "" ) fail;

        // sync
        tsys(gf + "sync");

        // check a/b content
		if( ol::file2str("a.txt" ) != "a45" ) fail;
		if( ol::file2str("b.txt" ) != "b45" ) fail;
    }

    // cleanup
	fs::remove_all("t12re");
	fs::remove_all("t12co");

    cout << "t10 12 OK\n";
}

void cmain13()
{
    cout << "cmix\n";
    // create dir t13re t13co
    // make file t13re/c.txt 123
    // make t13re/a.txt.cx
	// check .cx unpacks
    // in t13co make co
    // check c.txt content
    // update c content
    // sync
    // remove c
    // check c no exists
    // sync
    // check c content
    // cleanup

    string gf = "..\\..\\gf.exe ";

    // create dir t13re t13co
    fs::create_directory("t13re");
    fs::create_directory("t13co");

    {
        ol::Pushd pd("t13re");
        // make file t13re/a.txt 123; same for b.txt
        save("c.txt", "123");

        // make t13re/c.txt.cx
        tsys(gf + "cmix c.txt");

    	// check .cx unpacks
		fs::remove("c.txt");
		{ std::ofstream("c.txt.tmp")<<ol::file2str("c.txt.cx"); }
        tsys(gf + "unpack c.txt.cx");
		if( ol::file2str("c.txt") != "123" ) fail;
        ///tsys(gf + "cmix c.txt");
		///fs::remove("c.txt");
		fs::rename("c.txt.tmp","c.txt.cx");
///return; // FIXME cont
    }

    {
        ol::Pushd pd("t13co");
        // in t13co make co
        esys2(gf + "co ../t13re","../t13.out");
		fs::remove("../t13re/c.txt");
///return; // FIXME cont
        tsys(gf + "co ../t13re");

        // check c.txt content
		if( ol::file2str("c.txt" ) != "123" ) fail;

        // update c content
        save("c.txt", "c45");

        // sync
        tsys(gf + "sync");

        // remove c
		fs::remove("c.txt");

        // check c no exists
		if( ol::file2str("c.txt" ) != "" ) fail;

        // sync
        tsys(gf + "sync");

        // check c content
		if( ol::file2str("c.txt" ) != "c45" ) fail;
    }

    // cleanup
	fs::remove_all("t13re");
	fs::remove_all("t13co");

    cout << "t10 13 OK\n";
}

void cmain()
{
    cmain10();
    cmain11();
    cmain12();
    if(CHK_CMIX) cmain13();
}

#include "../hash.cpp"
#include "../sha2.cpp"
#include "../olu.cpp"

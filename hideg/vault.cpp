#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <memory>

#include "olu.h"
#include "gfu.h"
#include "hash.h"

using std::cout;

string g_vlt_name = ".gf.vlt";


int main_vault(ivec<string> args)
{
    if ( args.empty() )
    {
        cout << "hashing files\n";
        cout << "vault module creates hashes of files and directories\n";
        cout << "usage: vault {build|update|check|deep|same|clean}\n";
        cout << "build - rebuilds gf hashfiles\n";
        cout << "update - checks, and if needs updates hashfiles\n";
        cout << "check - verifiles that no changes in file tree\n";
        cout << "deep - validates hashes\n";
        cout << "same [dir1] ... - lists same files/trees in size order\n";
        cout << "clean - remove ["<<g_vlt_name<<"] files\n";
        return 0;
    }

    if ( args.size() < 1 ) throw "Bad arguments";

    string cmd = args[0];

	cout<<"AAA "<<cmd<<'\n';

    return 0;
}

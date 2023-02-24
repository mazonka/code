#include "ccrun.h"
#include "ccrut.h"

string g_src = " ";
string g_rep;
bool g_are = false;

//using namespace fsut;

void process_name(string name)
{
    static string sp = g_src;

    //cout << "processing file " << file << '\n';

    if ( g_rep.empty() )
    {
        auto i = name.find(sp);
        if ( i != string::npos )
        {
            cout << "[" << name << "]\n";
            g_are = true;
        }
        return;
    }

    string newn = name;
    bool withsp = ol::replaceAll(newn, sp, g_rep);

    if ( !withsp ) return;

    if ( fs::exists(newn) )
    {
        cout << "ERROR: file exists [" << newn << "]\n";
        return;
    }

    fs::rename(name, newn);

    cout << "[" << name << "] -> [" << newn << "]\n";
}

void process_dir(string dir)
{
    {
        //cout << "processing dir " << dir << '\n';
        fsut::Pushd pushd(dir);
        auto ents = fsut::readdir();

        for ( auto f : ents.files().names() ) process_name(f);
        for ( auto d : ents.dirs().names() ) process_dir(d);
    }

    // now maybe this dir is also need
    process_name(dir);
}

void cmain()
{
    ///cout<<args.size();
    if ( args.size() > 0 ) g_rep = args[0];
    if ( args.size() > 1 ) g_src = args[1];

    process_dir(".");

    if ( g_are && g_rep.empty() )
        cout << "\nspaces found\n";

    if ( g_rep.empty() )
    {
        cout << "use: findspace _ \n";
        cout << "     findspace _ -\n";
        cout << "     findspace @mapfile\n";
    }
}

#include "ccrun.h"
#include "ccrut.h"

string g_rep;
bool g_are = false;

//using namespace fsut;

void process_name(string name)
{
    //cout << "processing file " << file << '\n';

    if ( g_rep.empty() )
    {
        auto i = name.find(" ");
        if ( i != string::npos )
        {
            cout << "[" << name << "]\n";
            g_are = true;
        }
        return;
    }

    string newn = name;
    bool withsp = ol::replaceAll(newn, " ", g_rep);

    if ( !withsp ) return;

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

    process_dir(".");

    if ( g_are && g_rep.empty() )
    {
        cout << "\nspaces found; re-run with arg to replace\n";
        cout << "e.g. findspace _\n";
    }
}

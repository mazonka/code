#include "ccrun.h"
#include "ccrut.h"

bool g_act = false;

void process_dir(string dir, string pth)
{
    bool todel = false;

    {
        //cout << "processing dir/pth " << dir << ' '<< pth << '\n';
        fsut::Pushd pushd(dir);
        auto ents = fsut::readdir();

        //for ( auto f : ents.files().names() ) process_name(f);
        for ( auto d : ents.dirs().names() ) process_dir(d, pth.empty() ? d : pth + "/" + d);

        ents = fsut::readdir();
        todel = ents.empty();
    }

    if ( !todel || dir.empty() || dir == "." ) return;
    cout << (g_act ? "removing" : "empty");
    cout << " [" << pth << "]\n";
    if ( g_act ) fs::remove(dir);
}

void cmain_do()
{
    process_dir("", "");
}

void cmain()
{
    cout << "findempty finds empty dirs\n";
    cout << "use: findempty do - to delete\n";

    if ( !args.empty() )
    {
        g_act = true;
        if ( args[0] != "do" ) throw "use do";
        args.erase(args.begin());
    }

    cmain_do();
}


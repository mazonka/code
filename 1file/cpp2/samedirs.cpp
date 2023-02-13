#include <set>

#include "ccrun.h"
#include "ccrut.h"

const bool D = !true;

// number of the files threshold
int g_nsamefiles = 1;
int g_ndifffiles = 0;

//using namespace fsut;

std::map<string, fsut::Msul> g_m;

struct ResEntry
{
    string d1, d2;
    bool operator<(const ResEntry & b) const
    {
        return d1 < b.d1 ? true : b.d1 < d1 ? false : d2 < b.d2;
    }
};


struct Res : std::set<ResEntry>
{
    void add(string a1, string a2) { this->insert(ResEntry {a1, a2}); }
} g_res;

bool dcompare(const fsut::Msul & d1, const fsut::Msul & d2)
{
    auto f1 = d1.names();
    auto f2 = d2.names();

    std::sort(f1.begin(), f1.end());
    std::sort(f2.begin(), f2.end());

    if (D) for ( auto x : f1 ) cout << "f1: " << x << '\n';
    if (D) for ( auto x : f2 ) cout << "f2: " << x << '\n';

    auto i = f1.begin();
    auto j = f2.begin();

    int nsame = 0;

    while (1)
    {
        if ( i == f1.end() || j == f2.end() ) break;
        if ( *i == *j )
        {
            ++nsame; ++i; ++j;
            continue;
        }

        if ( *i < *j ) ++i;
        else ++j;
    }

    int ndiff = f1.size() + f2.size() - 2 * nsame;

    if (D)
        cout << "nsame >= g_nsamefiles && ndiff <= g_ndifffiles "
             << nsame << ' ' << g_nsamefiles << ' ' << ndiff << ' ' << g_ndifffiles
             << '\n';

    return nsame >= g_nsamefiles && ndiff <= g_ndifffiles;
}

void process_dir(string dir)
{
    {
        //cout << "processing dir " << dir << '\n';
        fsut::Pushd pushd(dir);
        auto ents = fsut::readdir();

        g_m[fs::current_path().string()] = ents.files();

        //for ( auto f : ents.files().names() ) process_name(f);
        for ( auto d : ents.dirs().names() ) process_dir(d);
    }
}

void cmain()
{
    cout << "samedirs [#SameFiles] [#DiffFiles]\n";

    ///cout<<args.size();
    if ( args.size() > 0 ) g_nsamefiles = std::stoi(args[0]);
    if ( args.size() > 1 ) g_ndifffiles = std::stoi(args[1]);

    process_dir(".");

    ///if ( g_are && g_rep.empty() )
    {
        //cout << "\nspaces found; re-run with arg to replace\n";
        //cout << "e.g. findspace _\n";
    }

    int cntr = 0 + (int)g_m.size();
    for ( auto i = g_m.begin(); i != g_m.end(); ++i )
    {
        cout << (--cntr) << " \r";
        auto j = i; ++j;
        for ( ; j != g_m.end(); ++j )
        {
            if(D) cout << " Comparing : " << i->first << ' ' << j->first << '\n';
            if ( dcompare(i->second, j->second) )
                g_res.add(i->first, j->first);
        }
    }

    if (D)
    {
        for ( auto x : g_m )
            cout << " [" << x.first << "] "
                 << x.second.dirs().size() << ' '
                 << x.second.files().size() << ' '
                 << "\n";
    }

    if (!g_res.empty())
    {
        cout << "Same dirs found:\n";
        for ( auto x : g_res )
            cout << "\n[" << x.d1 << "]\n"
                 << "[" << x.d2 << "]\n"
                 << "\n";
    }
    else
        cout << "No same dirs\n";
}


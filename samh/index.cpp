
#include "olc.h"
#include "index.h"

void main_index(ol::vstr & av)
{
    cout << "Command: "; for ( auto i : av ) cout << i << ' '; cout << '\n';

    if ( av.empty() ) av.push_back("help");

    string cmd = av[0];
    if ( !av.empty() ) av.erase(av.begin());

    if ( cmd == "help" )
    {
        std::cout << "index commands: gen\n";
        std::cout << "gen indexfile [cwd=.]\n";
        return;
    }

    else if ( cmd == "gen" )
    {
        void index_gen(ol::vstr & av);
        index_gen(av);
    }

    else if ( cmd == "same" )
    {
        void index_same(ol::vstr & av);
        index_same(av);
    }

    else throw "Bad index command [" + cmd + "]";
}

void index_gen(ol::vstr & av)
{
    if ( av.size() < 1 ) throw "index filemane expected";
    string indexfn = av[0];
    string cwd = ".";
    if ( av.size() > 1 ) cwd = av[1];
    extern bool inclDot;

    sam::mfu files = sam::getListOfFiles(cwd, inclDot);

    int sz = files.size();
    int cntr = 0;
    string prog;
    cout << '\n';

    std::ofstream of(indexfn);
    for ( auto fi : files )
    {
        cntr++;
        const auto & f =  fi.first;
        auto name = f.name();
        if ( name == indexfn ) continue;
        string qhash = sam::gethash(name, 100, true);
        string fhash = sam::gethash(name, ol::ull(-1), false);
        Hfile hf {f, qhash, fhash};
        of << hf.str() << '\n';

        string prg = ol::tos(int(100.0 * cntr / sz));
        if ( prg != prog )
        {
            prog = prg;
            cout << prog << "%\r";
        }
        ///cout << prog << ' '<< cntr << ' '<< sz <<"%\n";
    }
}

string Hfile::str() const
{
    ol::ostr os;
    auto dname = file.dname;
    if ( dname.empty() ) dname = "./";

    os << dname << '\n';
    os << file.fname << '\n';
    os << file.mtime << '\n';
    os << file.size << '\n';

    os << qhash << '\n';
    os << fhash << '\n';

    return os.str();
}

IndexFile::IndexFile(string f)
{
	throw "NI";
}

void index_same(ol::vstr & av)
{
    if ( av.size() < 1 ) throw "index filemane expected";
    string indexfn = av[0];

    IndexFile fi(indexfn);


}


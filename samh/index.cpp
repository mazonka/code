#include <fstream>

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

	std::ofstream of(indexfn,std::ios::binary);
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

IndexFile::IndexFile(string f) : filename(f)
{
    std::ifstream in(f);

    while (1)
    {
        string dir, name, mtime, ssize, qhash, fhash, x;
        std::getline(in, dir);
        std::getline(in, name);
        std::getline(in, mtime);
        std::getline(in, ssize);
        std::getline(in, qhash);
        std::getline(in, fhash);
        std::getline(in, x);
        if ( !in ) break;
        sam::File sf {dir, name, (time_t)std::stoull(mtime), std::stoull(ssize)};
        Hfile hf {sf, qhash, fhash};

        this->insert(hf);
    }

    ///throw "NI";
}

void IndexFile::save(string f) const
{
	std::ofstream of(f,std::ios::binary);
	for( const auto & i : (*this) ) of<<i.str()<<'\n';
}

void index_same(ol::vstr & av)
{
    if ( av.size() < 1 ) throw "index filemane expected";
    string indexfn = av[0];

    IndexFile fi(indexfn);

    cout << "Loaded index : " << fi.size() << '\n';

	std::map < string, std::vector<Hfile> > m;

	for( const auto & i : fi )
	{
		m[i.fhash].push_back(i);
	}

	// done with fi
	fi.clear();

	std::map < ol::ull, std::vector<Hfile> > b;

	for( const auto & i : m )
	{
		if( i.second.size() < 2 ) continue;
		b[i.second[0].file.size] = i.second;
	}

	// done with m
	m.clear();

	if( b.empty() )
	{
		cout<<"No same files found\n";
		return;
	}

	for( const auto & i : b )
	{
		cout<<"\nSame files of size: "<<i.first<<'\n';
		for( const auto & j : i.second ) cout<<j.file.name()<<'\n';
	}

	///fi.save();
}


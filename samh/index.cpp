#include <fstream>

#include "olc.h"
#include "index.h"

string qhash(sam::File f) { return sam::gethash(f.name(), 100, true); }
string fhash(sam::File f) { return sam::gethash(f.name(), ol::ull(-1), false); }

string qhcache(std::pair<const sam::File, QfHash> & a)
{
    auto & h = a.second.q;
    if ( h.empty() ) h = qhash(a.first);
    return h;
}

string fhcache(std::pair<const sam::File, QfHash> & a)
{
    auto & h = a.second.f;
    if ( h.empty() ) h = fhash(a.first);
    return h;
}


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

    else if ( cmd == "fix" || cmd == "validate" )
    {
        void index_fix(ol::vstr & av, bool isfix);
        index_fix(av, cmd == "fix");
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

    int sz = (int)files.size();
    int cntr = 0;
    string prog;
    cout << '\n';

    std::ofstream of(indexfn, std::ios::binary);
    for ( auto fi : files )
    {
        cntr++;
        const auto & f =  fi.first;
        auto name = f.name();
        if ( name == indexfn ) continue;
        ///string qhash = sam::gethash(name, 100, true);
        ///string fhash = sam::gethash(name, ol::ull(-1), false);
        string qh = qhash(f);
        string fh = fhash(f);
        Hfile hf { f, QfHash{qh, fh} };
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

string QfHash::str() const
{
    ol::ostr os;

    os << q << '\n';
    os << f << '\n';

    return os.str();
}

void cwd2slash(string & s, bool to)
{
///return;
    if (to)
    {
        if ( s.empty() ) s = "./";
    }
    else
    {
        if ( s == "./" ) s = "";
    }
}

string Hfile::str() const
{
    ol::ostr os;
    auto dname = file.dname;
    ///if ( dname.empty() ) dname = "./";
    cwd2slash(dname, true);

    os << dname << '\n';
    os << file.fname << '\n';
    os << file.mtime << '\n';
    os << file.size << '\n';

    os << hash.str();

    return os.str();
}

IndexFile::IndexFile(string f) : filename(f)
{
    std::ifstream in(f);
    if ( !in ) throw "Cannot open " + f;

    while (1)
    {
        string dir, name, mtime, ssize, qh, fh, x;
        std::getline(in, dir);
        ///if( dir == "./" ) dir == "";
        cwd2slash(dir, false);
        std::getline(in, name);
        std::getline(in, mtime);
        std::getline(in, ssize);
        std::getline(in, qh);
        std::getline(in, fh);
        std::getline(in, x);
        if ( !in ) break;
        sam::File sf {dir, name, (time_t)std::stoull(mtime), std::stoull(ssize)};

        (*this)[sf] = QfHash {qh, fh};
    }
}

void IndexFile::save(string f) const
{
    if ( f.empty() ) throw "Cannot save index file without name";
    std::ofstream of(f, std::ios::binary);
    for ( const auto & i : (*this) ) of << Hfile(i).str() << '\n';
}

void index_same(ol::vstr & av)
{
    if ( av.size() < 1 ) throw "index filemane expected";
    string indexfn = av[0];

    std::map < ol::ull, std::vector<Hfile> > b;
    {
        std::map < string, std::vector<Hfile> > m;
        {
            IndexFile fi(indexfn);

            cout << "Loaded index : " << fi.size() << '\n';

            for ( const auto & i : fi )
                m[i.second.f].push_back(Hfile {i});
        }

        for ( const auto & i : m )
        {
            if ( i.second.size() < 2 ) continue;
            b[i.second[0].file.size] = i.second;
        }
    }

    if ( b.empty() )
    {
        cout << "No same files found\n";
        return;
    }

    for ( const auto & i : b )
    {
        cout << "\nSame files of size: " << i.first << '\n';
        for ( const auto & j : i.second ) cout << j.file.name() << '\n';
    }

    ///fi.save();
}


void processCode(string code, IndexFile & idi, IndexFile & ifh, IndexFile & inf)
{
    cout << "Processing code: " << code << '\n';
    bool M = (code.find("M") == string::npos);
    bool R = (code.find("R") == string::npos);
    bool A = (code.find("A") == string::npos);
    bool H = (code.find("H") == string::npos);
    ol::replaceAll(code, "M", "");
    ol::replaceAll(code, "R", "");
    ol::replaceAll(code, "A", "");
    ol::replaceAll(code, "H", "");
    if ( !code.empty() ) throw "Bad MRAH-code";

    for ( auto & i : inf )
    {
        const auto & nf = i.first;
        Hfile candidate; int found = 0;
        for ( const auto & j : ifh )
        {
            const auto & hf = j.first;
///cout<<"AAA0 "<<nf.name()<< " = " <<hf.name()<<'\n';
            if ( nf.size != hf.size ) continue;
            if ( M && nf.dname != hf.dname ) continue;
            if ( R && nf.fname != hf.fname ) continue;
            if ( A && nf.mtime != hf.mtime ) continue;
            if ( H )
            {
                auto iter = idi.find(nf);
                if ( iter == idi.end() ) throw "Internal error 237";
                string qh = qhcache(*iter);
                ///cout<<"AAA " << hf.name() <<' '<< qh << " = " << j.second.q <<'\n';
                if ( qh != j.second.q ) continue;
            }

            ///cout << Hfile(nf).str() << '\n';
            ///throw "AAA";
            // we found a candidate
            candidate = Hfile(j);
            if ( ++found > 1 ) break;
        }

        if ( found != 1 ) continue;
        i.second = candidate.hash;

        inf.erase(nf);
        ifh.erase(nf);
    }
}

void index_fix(ol::vstr & av, bool isfix)
{
    if ( av.size() < 1 ) throw "index filename expected";
    string indexfn = av[0];
    string cwd = ".";
    if ( av.size() > 1 ) cwd = av[1];
    extern bool inclDot;

    if ( isfix )
    {
        cout << "MRAH-codes:";
        for ( int i = 2; i < av.size(); i++ ) cout << ' ' << av[i];
        cout << '\n';
    }

    // order of priority to check
    // 0. size - a must
    // 1. access
    // 2. fname
    // 3. dir

    // algorithm: Read files from dir and generate new index (DI) with
    // empty qh anf fh. Read index and put it in File:Hfile map (FH).
    // Go through each in DI and fill matching FH. On the way
    // construct non-matching list, and exclude matched from FH.
    // Next, for each MRAH-code find more matches.
    // Finally, calculate all hashes.
    // Save if fixflag

    sam::mfu files = sam::getListOfFiles(cwd, inclDot);

    IndexFile di;

    for ( auto fi : files )
    {
        const auto & f =  fi.first;
        auto name = f.name();
        if ( name == indexfn ) continue;
        di[f] = QfHash();
    }

    IndexFile fh(indexfn), notfound;

    for ( auto i : di )
    {
        auto file = i.first;
        auto found = fh.find(file);
        if ( found == fh.end() )
        {
            notfound[file];
        }
        else
        {
            di[file] = found->second;
            fh.erase(found);
        }
    }

    if ( fh.empty() && notfound.empty() )
    {
        cout << "Index matches\n";
        return;
    }

    for ( int i = 2; i < av.size(); i++ )
    {
        string code = av[i];
        processCode(code, di, fh, notfound);
    }

    if ( !isfix )
    {
        // validate
        cout << "Extra index found: " << fh.size() << '\n';
        //for ( auto i : fh ) cout << i.first.name() << '\n';

        cout << "Out of index: " << notfound.size() << '\n';
        for ( auto i : notfound )
            cout << i.first.name() << '\n';

        return;
    }


    cout << "Checking unresolved files\n";
    for ( auto & i : di )
    {
        ///const auto & file = i.first;
        ///auto & hash = i.second;
        (void)qhcache(i);
        (void)fhcache(i);
    }

    cout << "Saving index" << std::flush;
    di.save(indexfn);
    cout << " ok\n";
}


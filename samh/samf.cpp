#include "samf.h"
#include "copyfile.h"
#include "index.h"
#include "osfun.h"

using namespace sam;
using namespace ol;

using std::flush;

int g_useCache = 1;  // 0 do not use, 1 use, 2 remove

string sam::qhash(string f) { return sam::gethash(f, 100, true); }
string sam::fhash(string f) { return sam::gethash(f, ol::ull(-1), false); }

string sam::qhash(sam::File f) { return qhash(f.name()); }
string sam::fhash(sam::File f) { return fhash(f.name()); }

string sam::qhcache(std::pair<const sam::File, QfHash> & a)
{
    auto & h = a.second.q;
    if ( h.empty() ) h = qhash(a.first);
    return h;
}

string sam::fhcache(std::pair<const sam::File, QfHash> & a)
{
    auto & h = a.second.f;
    if ( h.empty() ) h = fhash(a.first);
    return h;
}


inline string cachename(string s, bool dot)
{
    return ".sam." + s + (dot ? ".dot" : "") + ".$";
}

sam::mfu loadSamCache(string prefix, string fn)
{
    sam::mfu r;
    IndexFile h(fn);
    for ( const auto & i : h )
    {
        ///r[i.first] = i.first.size;
        sam::File file = i.first;
        string & dn = file.dname;
        dn = prefix + dn;
        r.insert(std::make_pair(file, file.size));
    }
    return r;
}

sam::mfu sam::getListOfFilesR(os::Path p, bool dot)
{
    static ull sz = 0;
    static Timer timer;

    string pstr = p.str();
    string prefix = pstr + '/';
    if (pstr.empty()) never(1);
    if (pstr == ".") prefix = "";

    if ( g_useCache )
    {
        string b, dir;
        basename(pstr, b, dir);
        string cn = cachename(dir, dot);
        b += (b.empty() ? "" : "/");
        string cfn = b + cn;
        ///cout << "AAA cfn=[" << cfn << "]\n";
        if ( os::isFile(cfn) ) // cache exists
        {
            if ( g_useCache == 1 )
            {
                cout << "Using cache " << cfn << '\n';
                return loadSamCache(b, cfn);
            }
            cout << "Removing cache [" << cfn << "]" << flush;
            if ( !os::rmFile(cfn) ) throw "Failed to remove " + cfn;
            cout << " - ok\n";
        }
    }

    mfu r;
    os::Dir d = os::FileSys::readDirEx(p, true, true);

    sz += d.files.size();

    if ( timer.get() > 100 )
    {
        timer.init();
        cout << sz << '\r' << flush;
    }

    for ( auto f : d.files )
    {
        if ( !dot && f.first[0] == '.' ) continue;
        string n = prefix + f.first;
        File file {prefix, f.first, os::FileSys::mtime(n), ull(f.second)};
        r[file] = f.second;
    }

    for ( auto f : d.dirs )
    {
        if ( !dot && f[0] == '.' ) continue;
        auto n = getListOfFilesR(prefix + f, dot);
        r.insert(n.begin(), n.end());
    }

    return r;
}

sam::mfu sam::getListOfFiles(os::Path p, bool dot)
{
    cout << "Reading " << p.str() << '\n';
    auto r = getListOfFilesR(p, dot);
    cout << r.size() << '\n';
    return r;
}


void main_cache_make(ol::vstr & av)
{
    if ( av.size() < 1 ) throw "dirname";
    string dirname = av[0];

    if ( dirname == "." || dirname == ".."
            || dirname.find("/") != string::npos )
        throw "Dirname must be a simple name";

    cout << "Making cache for: " << dirname << '\n';

    extern bool inclDot;
    g_useCache = 0;
    sam::mfu files = sam::getListOfFiles(dirname, inclDot);

    IndexFile di;

    for ( auto fi : files ) di[fi.first] = QfHash {"=", "="};

    string cn = cachename(dirname, inclDot);

    cout << "Saving cache\r" << flush;
    di.save(cn);
    cout << "Cache [" << cn << "] saved, remove when required\n";
}

void main_cache(ol::vstr & av)
{
    if ( av.size() < 1 ) throw "Use: clean, to remove all | cache make DIR";

    string cmd = av[0];
    if ( !av.empty() ) av.erase(av.begin());

    if ( cmd == "make" ) return main_cache_make(av);

    if ( cmd != "clean" ) throw "cache: clean | make";

    if ( !av.empty() ) throw "clean is recursive and does not take args";

    extern bool inclDot;
    g_useCache = 2; // rm
    sam::mfu files = sam::getListOfFiles(".", inclDot);
}

void index_valid(ol::vstr & av)
{
    if ( av.size() < 2 ) throw "Use: valid {quick|full} index [dir]";
    string cmd = av[0];
    string indexfn = av[1];
    string cwd = ".";
    if ( av.size() > 2 ) cwd = av[2];
    extern bool inclDot;

    g_useCache = 0;
    cout << "Note: cache is not used during validation\n";

    bool full = false;
    if ( cmd == "quick" );
    else if ( cmd == "full" ) full = true;
    else throw "must be quick or false";

    sam::mfu files = sam::getListOfFiles(cwd, inclDot);

    IndexFile di;

    cout << "Resolving index" << flush;
    for ( auto fi : files )
    {
        const auto & f =  fi.first;
        auto name = f.name();
        if ( name == indexfn ) continue;
        di[f] = QfHash();
    }

    IndexFile fh(indexfn), notfound;
    int cntr = 0;

    for ( auto i : di )
    {
        auto file = i.first;
        auto found = fh.find(file);
        if ( found == fh.end() )
        {
            notfound[file];
            break;
        }
        else
        {
            di[file] = found->second;
            fh.erase(found);
            cntr++;
        }
    }

    if ( !notfound.empty() )
    {
        cout << "\nFailed: not all files indexed\n";
        return;
    }

    if ( !fh.empty() )
    {
        cout << "\nFailed: index file is not up-to-date\n";
        return;
    }

    cout << ": all " << cntr << " resolved\n";


    cout << "Validating " << di.size() << " files (press Esc to interrupt)\n";
    int disz = (int)di.size();
    cntr = 0;
    Timer timer;
    int invalids = 0;

    for ( const auto & i : di )
    {
        string bad;
        sam::File f = i.first;
        auto hash = i.second;

        if ( qhash(f) != hash.q ) bad = f.name();
        if ( full )
        {
            if ( fhash(f) != hash.f ) bad = f.name();
        }

        if ( !bad.empty() )
        {
            cout << "BAD: " << bad << '\n';
            ++invalids;
        }

        if ( os::kbhit() == 27 )
        {
            cout << "\nInterrupted, validation is not complete\n";
            break;
        }

        ++cntr;
        if ( timer.get() > 100 )
        {
            timer.init();
            cout << cntr << "/" << disz << '\r';
        }
    }

    if ( invalids )
        cout << "\nInvalid hashes found: " << invalids << '\n';
    else
        cout << cntr << "/" << disz << " ok\n";
}


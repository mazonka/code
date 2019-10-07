#include "samf.h"
#include "copyfile.h"
#include "index.h"

using namespace sam;
using namespace ol;

bool g_useCache = true;

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
            cout << "Using cache " << cfn << '\n';
            return loadSamCache(b, cfn);
        }
    }

    mfu r;
    os::Dir d = os::FileSys::readDirEx(p, true, true);

    sz += d.files.size();

    if ( timer.get() > 100 )
    {
        timer.init();
        cout << sz << '\r' << std::flush;
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
    return getListOfFilesR(p, dot);
}


void main_cache(ol::vstr & av)
{
    if ( av.size() < 1 ) throw "dirname";
    string dirname = av[0];

    if ( dirname == "." || dirname == ".."
            || dirname.find("/") != string::npos )
        throw "Dirname must be a simple name";

    cout << "Making cache for: " << dirname << '\n';

    extern bool inclDot;
    g_useCache = false;
    sam::mfu files = sam::getListOfFiles(dirname, inclDot);

    IndexFile di;

    for ( auto fi : files ) di[fi.first] = QfHash {"=", "="};

    string cn = cachename(dirname, inclDot);

    cout << "Saving cache\r" << std::flush;
    di.save(cn);
    cout << "Cache [" << cn << "] saved, remove when required\n";
}

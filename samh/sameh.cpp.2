// this is a Windows program (can be easily ported to Unix by Hasq dev)
// to find same files

#include "samehf.h"
#include "copyfile.h" // dirForFile

using namespace sam;
using namespace ol;

string kb(ull x)
{
    if ( x < 1000ull ) return tos(x);
    x /= 1000; if ( x < 1000ull ) return tos(x) + 'k';
    x /= 1000; if ( x < 1000ull ) return tos(x) + 'M';
    x /= 1000; if ( x < 1000ull ) return tos(x) + 'G';
    x /= 1000; return tos(x) + 'T';
}

string cache_name(ull sz)
{
    string name = ".sameh/" + kb(sz) + ".hash";
    return name;
}

void mymain(ull sz, bool chknames)
{
    // idea - first get the list of files
    // remove files with unique size and with zero size
    // do not read '.'-directories
    // place remaining into size buckets
    // for each bucket read first 100 bytes
    // eliminate uniques
    // set SZ = 1k
    // in each size bucket read SZ and hash
    // eliminate with unique hashes
    // those that size<SZ declare SAME
    // make size-hash-buckets (SH)
    // set SZ *= 10
    // repeat

    extern bool inclDot;
    mfu allfiles = sam::getListOfFiles(".", inclDot);
    cout << "Read: " << allfiles.size() << " files\n";
    //for ( auto x : allfiles ) cout << x.first << '\t' << x.second << '\n';

    cout << "Removing unique size" << (chknames ? ",names " : "")
         << " and 0-size files\n";

    {
        std::map<ull, ull> au;
        std::map<string, ull> as;

        for ( auto x : allfiles )
        {
            as[x.first.fname] += 1;
            au[x.second] += 1;
        }

        mfu b;
        for ( auto x : allfiles )
        {
            ull s = x.second;
            if ( s == 0 ) continue;
            if ( au[s] == 1 ) continue;
            if ( chknames && as[x.first.fname] == 1 ) continue;
            b.insert(x);
        }
        b.swap(allfiles);
    }
    cout << "Remained: " << allfiles.size() << " files\n";
    //for ( auto x : allfiles ) cout << x.first << '\t' << x.second << '\n';

    mshvf buckets;
    for ( auto x : allfiles )
    {
        string key = tos(x.second); // size only
        if (chknames) key = x.first.sname(); //size and name
        SH s {key, ""};
        buckets[s].push_back(x.first);
    }
    cout << "Made " << buckets.size() << " buckets\n";
    //prn_buckets(buckets);

    // release allfiles
    allfiles.clear();

    mshvf same;
    for (ull SZ = 100; SZ <= sz && !buckets.empty(); SZ *= 100 )
    {

        //count files
        ull fcnt = 0;
        for ( auto & buck : buckets )
            for ( auto f : buck.second ) fcnt++;

        cout << "Checking " << fcnt << " files for size " << kb(SZ) << std::flush;

        mshvf newb;
        ull cntr = 0;
        Timer timer;
        for ( auto & buck : buckets )
        {
            for ( auto f : buck.second )
            {
                SH h { buck.first.ssize, gethash_cache(f, SZ, SZ == 100) };
                newb[h].push_back(f);

                ++cntr;
                //if ( ( cntr += SZ ) > PRN )
                //{ cout << "." << std::flush; cntr = 0; }
                if ( timer.get() / 1000 > 1)
                {
                    timer.init();
                    cout << "\rChecking " << cntr << "/" << fcnt
                         << " files for size " << kb(SZ) << " " << std::flush;
                }
            }
        }

        cout << "\rChecked " << cntr << "/" << fcnt
             << " files for size " << kb(SZ) << std::flush;
        cout << "   " << std::flush;

        {
            string name = cache_name(SZ);
            cout << "\nWrite cache " << name;
            save_buckets(name, newb);
        }

        buckets.clear();
        for ( auto x : newb )
            if ( x.second.size() > 1 )
            {
                if ( SZ < x.second[0].size )
                    buckets.insert(x);
                else
                    same.insert(x);
            }

        cout << "\n";
    } // next SZ

    cout << "\n";

    if ( !same.empty() )
    {
        cout << "Same files found, ";
        prn_buckets(same);
    }
    else
    {
        cout << "No same files found";
        if ( !buckets.empty() ) cout << ", BUT";
        cout << "\n";
    }

    if ( !buckets.empty() )
    {
        cout << "Unchecked files exceeded "
             << kb(sz) << "\n";

        prn_buckets(buckets);
    }
}

void main_sameh(vector<string> & args)
{
    cout << "Usage: sameh [[@]SizeKb]\n";
    cout << "@ - compare only file with the same names\n";

    ull sz = SZMAX;
    bool chknames = false;

    if ( args.size() > 0 )
    {
        string ssz = args[0];
        if ( ssz[0] == '@' ) { chknames = true; ssz = ssz.substr(1); }
        sz = std::stoi(ssz) * 1000ull;
    }

    mymain(sz, chknames);
}

int main_sameh(int ac, char * av[])
try
{
    try
    {
        try
        {
            cout << "Usage: sameh [SizeKb]\n";
            ull sz = SZMAX;
            if ( ac > 1 ) sz = std::atoi(av[1]) * 1000ull;
            mymain(sz, false);
        }
        catch (...)
        {
            cout << "Current dir: [" << os::FileSys::cwd().str() << "]\n";
            throw;
        }
    }
    catch (int e) { throw "[" + tos(e) + "]"; }
    catch (const char * e) { throw string(e); }
    return 0;
}
catch (string e)
{
    cout << "Error: " << e << "\n";
    return 1;
}
catch (std::exception & e)
{
    cout << "Error (C++ exception) : " << e.what() << '\n';
    return 2;
}
catch (...)
{
    cout << "Error: unknown exception\n";
    return 1;
}

void cfn_read(std::istream & is, string & dest)
{
    const int BSZ = 1000;

    char buf[BSZ + 1];

    is.read(buf, BSZ);

    auto sz = static_cast<unsigned>(is.gcount());
    string x(buf, sz);

    dest.swap(x);
}

string sam::gethash(string f, ull sz, bool three)
try
{
    using ma::skc::hashHex;

    std::ifstream in(f, std::ios::binary);
    if ( !in ) throw "Cannot open [" + f + "]";

    ull size = 0;
    string r;

    string cs;
    while (1)
    {
        cs.clear();
        cfn_read(in, cs);
        if ( !cs.empty() )
        {
            size += cs.size();
            if ( size > sz )
                r += cs.substr(0, static_cast<unsigned>(cs.size() + sz - size));
            else
                r += cs;

            if ( size >= sz ) break;
        }

        const size_t MX = 1000000;
        if ( r.size() > MX )
            r = hashHex(r.substr(0, MX)) + ':' + r.substr(MX);

        if ( !in ) break;
    }

    r = hashHex(r);

    if ( three ) // read last 100 bytes
    {
        // read last bytes
        in.clear();
        in.seekg(0, std::ios::end);
        ull fsz = in.tellg();

        const int BLK = 1000;
        if ( fsz < 4 * BLK )
        {
            in.close();
            return gethash(f, 4 * BLK, false);
        }

        in.seekg((fsz - BLK) / 2, std::ios::beg);
        cfn_read(in, cs);
        if ( cs.size() > BLK ) cs = cs.substr(0, BLK);
        r += ":" + hashHex(cs);

        in.seekg(-BLK, std::ios::end);
        cfn_read(in, cs);
        r += ":" + hashHex(cs);

        r = hashHex(r);
    }

    return r;
}
catch (...)
{
    cout << "\nExceptioin while reading file [" << f << "]\n";
    throw;
}

void sam::prn_buckets(mshvf & buckets)
{
    cout << "buckets: " << buckets.size() << "\n\n";

    std::map<ull, vector<vector<File> > > sb;
    for ( auto x : buckets )
    {
        sb[x.second[0].size].push_back(x.second);
    }

    for ( auto y : sb )
    {
        auto sv = y.second;
        for ( auto x : sv )
        {
            auto sz = x.size();
            if (!sz) never("empty bucket");
            cout << "size=" << x[0].size << "\tfiles=" << x.size() << "\n";

            for ( auto i = 0 * sz; i < sz; i++ )
                cout << "[" << (i + 1) << "] " << x[i].name() << '\n';

            cout << "\n";
        }
    }
}

void sam::save_buckets(string name, mshvf & newb)
{
    dirForFile(name);
    std::ofstream uf(name);

    if ( !uf ) throw "Cannot open " + name;

    uf << newb.size() << "\n";

    std::map<ull, string> blocks;

    for ( auto b : newb )
    {
        ostr of;
        auto sh = b.first;
        auto & vf = b.second;
        if ( vf.empty() ) never("empty entry");

        auto siz = vf[0].size;
        of << '\n' << siz << ' ' << sh.hash << '\n';

        of << vf.size() << '\n';
        for ( auto f : vf )
            of << f.name() << '\n' << f.mtime << '\n';

        blocks[siz] = of.str();
    }

    for ( auto x : blocks ) uf << x.second;
}

mfs load_cache(string name)
{
    mfs r;

    std::ifstream in(name);
    if ( !in ) return r;

    int sz = 0;
    string ln;
    using std::getline;
    getline(in, ln); istr(ln) >> sz;

    for ( int i = 0; i < sz; i++ )
    {
        ull size = 0;
        string hash;
        int nfiles;

        // size and hash
        getline(in, ln); getline(in, ln);
        istr(ln) >> size >> hash;

        // nfiles
        getline(in, ln); istr(ln) >> nfiles;

        // load files: name, mtime
        for ( int j = 0; j < nfiles; j++ )
        {
            string file;
            time_t mtime;
            getline(in, ln); file = eatSpaces(ln);
            getline(in, ln); istr(ln) >> mtime;

            string fname, dname;
            auto k = file.rfind('/');
            if ( k == string::npos) fname = file;
            else
            {
                fname = file.substr(k + 1);
                dname = file.substr(0, k);
            }

            File fs {dname, fname, mtime, size};
            r[fs] = hash;
        }
    }

    return r;
}

string sam::gethash_cache(File f, ull sz, bool three)
{
    static std::map<string, mfs> caches;

    string name = cache_name(sz);

    auto icache = caches.find(name);
    if ( icache == caches.end() )
    {
        caches[name] = load_cache(name);
        icache = caches.find(name);
    }

    auto & cache = icache->second;

    auto ifile = cache.find(f);

    if ( ifile == cache.end() )
        return gethash(f.name(), sz, three);

    return ifile->second;
}


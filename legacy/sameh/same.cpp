// this is a Windows program (can be easily ported to Unix by Hasq dev)
// to find same files
// this is older program, newer is 'sameh'

#include "ol.h"
#include "os_filesys.h"
#include "timer.h"

const ull SZMAX = 10ull * 1000 * 1000 * 1000;
const ull PRN = 1ull * 1000 * 100;

using msu = std::map<string, ull>;

struct SH
{
    ull size;
    string hash;
    bool operator<(const SH & x) const
    {
        if ( size < x.size ) return true;
        if ( size > x.size ) return false;
        return hash < x.hash;
    }
};

string gethash(string f, ull sz);
msu getListOfFiles(os::Path p);
void prn_buckets(std::map<SH, vstr> & buckets);

string kb(ull x)
{
    if ( x < 1000ull ) return tos(x);
    x /= 1000; if ( x < 1000ull ) return tos(x) + 'k';
    x /= 1000; if ( x < 1000ull ) return tos(x) + 'M';
    x /= 1000; if ( x < 1000ull ) return tos(x) + 'G';
    x /= 1000; return tos(x) + 'T';
}


void mymain(ull sz)
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

    msu allfiles = getListOfFiles(".");
    cout << "Read: " << allfiles.size() << " files\n";
    //for ( auto x : allfiles ) cout << x.first << '\t' << x.second << '\n';

    cout << "Removing unique size and 0-size files\n";
    {
        std::map<ull, ull> a;
        for ( auto x : allfiles ) a[x.second] += 1;
        msu b;
        for ( auto x : allfiles )
        {
            ull s = x.second;
            if ( s == 0 ) continue;
            if ( a[s] == 1 ) continue;
            b.insert(x);
        }
        b.swap(allfiles);
    }
    cout << "Remained: " << allfiles.size() << " files\n";
    //for ( auto x : allfiles ) cout << x.first << '\t' << x.second << '\n';

    std::map<SH, vstr> buckets;
    for ( auto x : allfiles )
    {
        SH s {x.second, ""};
        buckets[s].push_back(x.first);
    }
    cout << "Made " << buckets.size() << " buckets\n";
    //prn_buckets(buckets);

    // release allfiles
    allfiles.clear();

    std::map<SH, vstr> same;
    for (ull SZ = 1; SZ <= sz && !buckets.empty(); SZ *= 10 )
    {

        //count files
        ull fcnt = 0;
        for ( auto & buck : buckets )
            for ( auto f : buck.second ) fcnt++;

        cout << "Checking " << fcnt << " files for size " << kb(SZ) << std::flush;

        std::map<SH, vstr> newb;
        ull cntr = 0;
        Timer timer;
        for ( auto & buck : buckets )
        {
            for ( auto f : buck.second )
            {
                SH h { buck.first.size, gethash(f, SZ) };
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

        buckets.clear();
        for ( auto x : newb )
            if ( x.second.size() > 1 )
            {
                if ( SZ < x.first.size )
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

int main(int ac, char * av[])
try
{
    try
    {
        try
        {
            cout << "Usage: same [SizeKb]\n";
            ull sz = SZMAX;
            if ( ac > 1 ) sz = std::atoi(av[1]) * 1000ull;
            mymain(sz);
        }
        catch (...)
        {
            cout << "Current dir: [" << os::FileSys::cwd().str() << "]\n";
            throw;
        }
    }
    catch (int e) { throw "[" + tos(e) + "]"; }
    catch (const char * e) { throw string(e); }
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


msu getListOfFiles(os::Path p)
{
    static ull sz = 0;

    string pstr = p.str();
    string prefix = pstr + '/';
    if (pstr.empty()) never(1);
    if (pstr == ".") prefix = "";

    msu r;
    os::Dir d = os::FileSys::readDirEx(p, true, true);

    sz += d.files.size();
    cout << sz << '\r' << std::flush;

    for ( auto f : d.files )
    {
        if ( f.first[0] == '.' ) continue;
        r[prefix + f.first] = f.second;
    }
    for ( auto f : d.dirs )
    {
        if ( f[0] == '.' ) continue;
        auto n = getListOfFiles(prefix + f);
        r.insert(n.begin(), n.end());
    }

    return r;
}

void cfn_read(std::istream & is, string & dest)
{
    const int BSZ = 1000;

    char buf[BSZ + 1];

    is.read(buf, BSZ);

    auto sz = is.gcount();
    string x(buf, sz);

    dest.swap(x);
}

string gethash(string f, ull sz)
try
{
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
                r += cs.substr(0, cs.size() + sz - size);
            else
                r += cs;

            if ( size >= sz ) break;
        }

        if ( !in ) break;
    }

    if ( sz == 1ull && size > 100 ) // read last 100 bytes
    {
        // read last bytes
        in.clear();
        in.seekg(-100, std::ios::end);
        cfn_read(in, cs);
        r += cs;
    }

    return r;
}
catch (...)
{
    cout << "\nExceptioin while reading file [" << f << "]\n";
    throw;
}

void prn_buckets(std::map<SH, vstr> & buckets)
{
    cout << "buckets: " << buckets.size() << "\n\n";

    for ( auto x : buckets )
    {
        cout << "size=" << x.first.size << "\tfiles=" << x.second.size() << "\n";
        auto sz = x.second.size();

        for ( auto i = 0 * sz; i < sz; i++ )
            cout << "[" << (i + 1) << "] " << x.second[i] << '\n';

        cout << "\n";
    }
}


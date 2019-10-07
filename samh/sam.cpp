#include <fstream>

#include "olc.h"

#include "samf.h"
#include "copyfile.h"
#include "index.h"


bool inclDot = false;

void olmain_func(ol::vstr & av);
void olmain(ol::vstr & av);
void olmain1(ol::vstr & av)
{
    if ( av.size() < 1 )
    {
        std::cout << "Usage: longname, cutname, index, sameh, [no]copy, @file\n";
        std::cout << "Use @ (e.g. sameh@) to include '.' files and dirs\n";
        std::cout << "sameh - find same files in the current dir\n";
        std::cout << "copy D1 D2 D3 - copies files D1 to D2 moving from D3\n";
        std::cout << "nocopy - checks files D1 by moving from D3 to D2\n";
        std::cout << "nocopy - same as copy but does not copy files\n";
        std::cout << "longname - prints long file names\n";
        std::cout << "cutname ext pos size - cut long filenames\n";
        return;
    }

    olmain_func(av);
}

void olmain0(ol::vstr & av)
{
    if ( av.size() < 2 )
    {
        std::cout << "sam, Oleg Mazonka, 2018-2019, " VERSION "\n";
        std::cout << "Usage: func, gen, fix, valid, same, rmsame, split,\n";
        std::cout << "       split, @file, hash, link, addindex, cache\n";
        std::cout << "Use @ to include '.' files and dirs\n";
        return;
    }

    if ( !av.empty() ) av.erase(av.begin());
    olmain(av);
}


void olmain(ol::vstr & av)
{
    string cmd = av[0];
    if ( !av.empty() ) av.erase(av.begin());

    if (cmd == "@" )
    {
        inclDot = true;
        return olmain(av);
    }

    if ( 0 ) {}

    else if ( cmd[0] == '@' )
    {
        void main_file(string);
        main_file(cmd.substr(1));
    }

    else if ( cmd == "func" )
    {
        olmain1(av);
    }

    else if ( cmd == "gen" || cmd == "fix" || cmd == "split"
              || cmd == "valid" || cmd == "same"
              || cmd == "rmsame" || cmd == "addindex" )
    {
        auto vcmd = ol::vstr {cmd} +av;
        void main_index(ol::vstr &);
        main_index(vcmd);
    }

    else if ( cmd == "hash" )
    {
        void main_hash(ol::vstr &);
        main_hash(av);
    }

    else if ( cmd == "link" )
    {
        void main_link(ol::vstr &);
        main_link(av);
    }

    else if ( cmd == "cache" )
    {
        void main_cache(ol::vstr &);
        main_cache(av);
    }

    else throw "sam: unknown command [" + cmd + "]";
}

void olmain_func(ol::vstr & av)
{

///    for ( auto i : av ) cout << i << '\n';

    string cmd = av[0];
    if ( !av.empty() ) av.erase(av.begin());

    {
        auto last = cmd.size() - 1;
        if (cmd[last] == '@' )
        {
            inclDot = true;
            cmd = cmd.substr(0, last);
        }
    }

    void main_copycond(ol::vstr &, bool);

    if ( 0 ) {}

    else if ( cmd == "longname" )
    {
        void main_longname(ol::vstr &);
        main_longname(av);
    }

    else if ( cmd == "cutname" )
    {
        void main_cutname(ol::vstr &);
        main_cutname(av);
    }

    else if ( cmd == "sameh" )
    {
        void main_sameh(ol::vstr &);
        main_sameh(av);
    }

    else if ( cmd == "copy" ) main_copycond(av, true);
    else if ( cmd == "nocopy" ) main_copycond(av, false);

    else throw "sam: unknown command [" + cmd + "]";
}

// list file with full names longer than 100 or N bytes
void main_longname(ol::vstr & args)
{
    std::cout << "Usage: longname [sizeB=100]\n";
    size_t size = 100;
    if ( args.size() > 0 ) size = std::stoi(args[0]);

    auto flist = sam::getListOfFiles(".", inclDot);

    int count = 0;
    size_t maxsz = 0;
    string maxnm;
    for ( const auto & entry : flist )
    {
        string name = entry.first.name();
        auto sz = name.size();
        if ( sz > maxsz ) { maxsz = sz; maxnm = name; }
        if ( name.size() > size )
        {
            count++;
            std::cout << name << '\n';
        }
    }

    if ( !count ) std::cout << "No long names found\n";
    else std::cout << "===\n";

    std::cout << "The longest is " << maxsz << '\n' << maxnm << '\n';
}

// cuts long file names
void main_cutname(ol::vstr & args)
{
    if ( args.size() < 3 )
    {
        std::cout << "Usage: ext pos size\n";
        std::cout << "ext - file extention, '.' for no ext\n";
        std::cout << "pos - start position, cut before\n";
        std::cout << "size - max name length, 0 - for end\n";
        return;
    }

    string ext = "." + args[0];
    int pos = std::stoi(args[1]);
    int size = std::stoi(args[2]);

    auto flist = sam::getListOfFiles(".", inclDot);

    std::map<string, vector<sam::File>> dirs;
    for ( const auto & e : flist ) dirs[e.first.dname].push_back(e.first);

    std::map<sam::File, sam::File> oldnew;
    for ( const auto & x : dirs )
    {
        auto dir = x.first;
        auto vecf = x.second;

        vector<sam::File> cutname_filter(const vector<sam::File> &, string);
        auto vf = cutname_filter(vecf, ext);
        if ( vf.empty() ) continue;

        //cout << "DIR: " << dir << '\n';
        //for ( const auto & y : vf ) cout<< " [" << y.fname<<"]"; cout << '\n';

        auto nf = vf;
        void cutstrname(string & s, int, int, string);
        for ( auto & x1 : nf ) cutstrname(x1.fname, pos, size, ext);

        // check uniqueness
        {
            std::map<string, int> m;
            for ( auto & x2 : nf )
            {
                if ( (++m[x2.fname]) > 1 )
                    throw "Name [" + x2.fname + "] in [" + dir + "] not unique";
            }
        }

        //for ( const auto & y : nf ) cout<< " [" << y.fname<<"]"; cout << '\n';
        for ( size_t i = 0; i < nf.size(); i++ )
            oldnew[vf[i]] = nf[i];
    }

    for ( auto x : oldnew )
    {
        auto of = x.first.name();
        auto nf = x.second.name();
        if ( nf == of ) continue;
        cout << of << " -> " << nf << '\n';
        os::FileSys::move(of, nf);
    }
}

vector<sam::File> cutname_filter(const vector<sam::File> & vf, string ext)
{
    if ( ext == ".." ) return vf;
    vector<sam::File> r;

    for ( auto x : vf )
    {
        string n = x.fname;
        auto sz = n.size();
        auto ez = ext.size();
        if ( sz <= ez ) continue;
        if ( n.substr(sz - ez) == ext ) r.push_back(x);
    }
    return r;
}

void cutstrname(string & s, int pos, int size, string ext)
{
    auto sz = s.size();
    auto ez = ext.size();

    auto n = s.substr(0, sz - ez);

    if ( (int)n.size() <= size ) return;
    if ( pos >= (int)n.size() ) pos -= size;
    if ( pos < 0 ) pos = 0;

    s = n.substr(pos, size ? size : string::npos) + ext;
}

void main_file(string file)
{
    std::ifstream in(file);
    if ( !in ) throw "Cannot open " + file;

    for ( string line; std::getline(in, line); )
    {
        ol::cutline(line);
        if ( line.empty() ) continue;
        if ( line[0] == '#' ) continue;
        ol::vstr v = ol::str2vstr(line, " ");
        olmain(v);
    }
}

// conditional copying of files
// dir_from dir_to [dir_cache]
string newfile(const sam::File & fs, string src, string dst);
void main_copycond(ol::vstr & args, bool docopy)
{
    string src, dst, cch;

    if (args.size() < 2) throw "Usage: copycond dirFrom dirTo [dirCache]";
    sam::mfu listCache;

    src = args[0];
    dst = args[1];
    if ( src == dst ) throw "Bad source and destination";

    if (args.size() > 2)
    {
        cch = args[2];
        if ( src == cch || dst == cch ) throw "Bad cache";

        std::cout << "reading cache\n";
        listCache = sam::getListOfFiles(cch, inclDot);
        std::cout << "Cache: " << listCache.size() << '\n';
    }

    std::cout << "reading source\n";
    auto listSrc = sam::getListOfFiles(src, inclDot);
    std::cout << "Source: " << listSrc.size() << '\n';

    std::cout << "reading destination\n";
    auto listDst = sam::getListOfFiles(dst, inclDot);
    std::cout << "Dest: " << listDst.size() << '\n';

    std::map<string, int> cache_weak;
    std::map<string, sam::File> cache_full;
    for (const auto & x : listCache)
    {
        cache_weak[x.first.sname()] += 1;
        cache_full[x.first.name()] = x.first;
    }

    void copycond(string src, string dst, bool copy);

    auto prn = [](int c, size_t sz, string f)
    {
        while (f.size() > 40)
            f = f.substr(0, 20) + "..."
                + f.substr(f.size() - 15);
        while (f.size() < 40) f += " ";
        std::cout << c << '/' << sz << ' ' << f << '\r';
    };

    int counter = 0;
    Timer timer;

    const bool DB = false;

    for (const auto & from : listSrc)
    {
        auto file = from.first;

        ++counter;
        if ( timer.get() > 200 )
        {
            timer.init();
            prn(counter, listSrc.size(), file.name());
        }

        string newf = newfile(file, src, dst);
        string cchf = newfile(file, src, cch);

        auto sn = file.sname();

        if ( os::Path(newf).isfile() ) continue;
        else // try read
        {
            std::ifstream test(newf, std::ios::binary);
            if ( !!test ) continue;
        }

        auto incachew = cache_weak.find(sn);
        auto incachef = cache_full.find(cchf);

        if (0) {}
        else if ( incachef != cache_full.end()
                  && incachef->second.size == file.size )
        {
            sam::File cfile = incachef->second;
            copycond(cfile.name(), newf, false);
            listCache.erase(cfile);
            cache_full.erase(cchf);
            if (DB) cout << "E " << newf << '\n';
            continue;
        }
        else if ( incachew != cache_weak.end() && incachew->second == 1 )
        {
            sam::File cfile;
            bool found = false;
            {
                for (const auto & x : listCache)
                {
                    if ( x.first.sname() != sn ) continue;
                    found = true;
                    cfile = x.first;
                }
            }

            if ( !found ) never("bad cache");

            copycond(cfile.name(), newf, false);
            listCache.erase(cfile);
            --incachew->second;
            if (DB) cout << "U " << newf << '\n';
            continue;
        }
        else // need to copy
        {
            if ( file.size > 10000000 )
                prn(counter, listSrc.size(), "*" + file.name());

            if (DB) cout << "C " << newf << '\n';
            if (docopy) copycond(file.name(), newf, true);
        }
    }
}

string newfile(const sam::File & fs, string src, string dst)
{
    string fsname = fs.name();
    if ( src != fsname.substr(0, src.size()) )
        throw "Bad prefix [" + src + "] [" + fsname + "]";

    return dst + fsname.substr(src.size());
}

void copycond(string fsname, string newname, bool copy)
{
    dirForFile(newname);
    if (copy) copyfile(fsname, newname);
    else os::FileSys::move(fsname, newname);
}

// file sameh.cpp START

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

// END FILE


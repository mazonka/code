
#include "samehf.h"
#include "copyfile.h"

bool inclDot = false;

void olmain(vector<string> & av)
{
    if ( av.size() < 2 )
    {
        std::cout << "sam, Oleg Mazonka, 2018, v0.5\n";
        std::cout << "Use commands: sameh, [no]copy, longname\n";
        std::cout << "Use @ (e.g. sameh@) to include '.' files and dirs\n";
        std::cout << "sameh - find same files in the current dir\n";
        std::cout << "copy D1 D2 D3 - copies files D1 to D2 moving from D3\n";
        std::cout << "nocopy - checks files D1 by moving from D3 to D2\n";
        std::cout << "nocopy - same as copy but does not copy files\n";
        std::cout << "longname - prints long file names\n";
        std::cout << "cutname ext pos size - cut long filenames\n";
        return;
    }

    string cmd = av[1];
    if ( !av.empty() ) av.erase(av.begin());
    if ( !av.empty() ) av.erase(av.begin());

    {
        auto last = cmd.size() - 1;
        if (cmd[last] == '@' )
        {
            inclDot = true;
            cmd = cmd.substr(0, last);
        }
    }

    if ( cmd == "longname" )
    {
        void main_longname(vector<string> &);
        main_longname(av);
        return;
    }

    if ( cmd == "sameh" )
    {
        void main_sameh(vector<string> &);
        main_sameh(av);
        return;
    }

    if ( cmd == "cutname" )
    {
        void main_cutname(vector<string> &);
        main_cutname(av);
        return;
    }

    void main_copycond(vector<string> &, bool);
    if ( cmd == "copy" )
    {
        main_copycond(av, true);
        return;
    }
    if ( cmd == "nocopy" )
    {
        main_copycond(av, false);
        return;
    }

    throw "sam: unknown command [" + av[1] + "]";
}

// list file with full names longer than 100 or N bytes
void main_longname(vector<string> & args)
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

// conditional copying of files
// dir_from dir_to [dir_cache]
string newfile(const sam::File & fs, string src, string dst);
void main_copycond(vector<string> & args, bool docopy)
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


// cuts long file names
void main_cutname(vector<string> & args)
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
        for ( auto & x : nf ) cutstrname(x.fname, pos, size, ext);

        // check uniqueness
        {
            std::map<string, int> m;
            for ( auto & x : nf )
            {
                if ( (++m[x.fname]) > 1 )
                    throw "Name [" + x.fname + "] in [" + dir + "] not unique";
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

    if ( n.size() <= size ) return;
    if ( pos >= (int)n.size() ) pos -= size;
    if ( pos < 0 ) pos = 0;

    s = n.substr(pos, size ? size : string::npos) + ext;
}


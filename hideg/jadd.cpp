#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>

#include "olu.h"
#include "gfu.h"
#include "hash.h"

using std::cout;

string g_cache_name = "gf.jadd.log";
string g_snap_name = "gf.snap.log";

struct File
{
    unsigned long long sz = 0;
    string hashHead;
    string hashFile;

    fs::path pth;
    unsigned long long tc = 0;

    void print() const;
    static bool same(fs::path dira, File & a, fs::path dirb, File & b);
};

struct Files
{
    fs::path dir;
    ivec<File> files;
    std::map<unsigned long long, ivec<size_t> > sz2idx;

    void add(File f) { sz2idx[f.sz].push_back(files.size()); files.push_back(f); }
    void print() const;
};

Files loadFrom(fs::path dir);
Files loadCache(string file, bool read);
void copy2dest(fs::path dir_dst, fs::path dir_src, fs::path file);
void saveCache(const Files & files);

int main_jadd(ivec<string> args)
{
    if ( args.empty() )
    {
        cout << "journaling file copy\n";
        cout << "jadd module copies files from SRC to DST\n";
        cout << "only thouse that do not exist in TRG\n";
        cout << "usage: fadd {TRG|[@]cache} SRC DST\n";
        cout << "TRG - target directory, SRC - source, DTS - destination\n";
        cout << "cache - is the file produced by fadd\n";
        cout << "use @ (@filename) to skip reading TRG\n";
        return 0;
    }

    if ( args.size() != 3 ) throw "Bad arguments";

    string TRG = args[0];
    string SRC = args[1];
    string DST = args[2];

    bool skipRead = false;
    bool srcFile = false;

    if ( TRG.size() && TRG[0] == '@' )
    {
        skipRead = true;
        TRG = TRG.substr(1);
    }

    if ( !fs::exists(TRG) ) throw "no " + TRG;
    if ( !fs::exists(SRC) ) throw "no " + SRC;

    Files tFiles, sFiles;

    if ( !fs::is_directory(SRC) ) srcFile = true;
    if ( !fs::is_directory(TRG) )
    {
        tFiles = loadCache(TRG, skipRead);
        //tFiles.print();
        //never;
    }
    else tFiles = loadFrom(TRG);

    if ( srcFile )
    {
        // sFiles.add(SRC);
        throw "Adding single file is not yet supprted, use dir\n";
    }
    else sFiles = loadFrom(SRC);

    int cntr1 = 0, cntr2 = 0;
    for ( auto & sf : sFiles.files )
    {
        ///cout << ' ' << sf.pth.string() << "\n";
        // get list of files from TRG to compare
        auto tidxs = tFiles.sz2idx[sf.sz];
        bool isnew = true;
        for ( int i : tidxs )
        {
            auto & tf = tFiles.files[i];

            if ( File::same(sFiles.dir, sf, tFiles.dir, tf) )
            {
                isnew = !true;
                break;
            }
        }

        if ( isnew ) { ++cntr1; copy2dest(DST, SRC, sf.pth); }

        cout << (++cntr2) << "/" << sFiles.files.size() << "\r";
    }

    cout << "copied " << cntr1 << " out of " << sFiles.files.size() << " files\n";

    saveCache(tFiles);

    if (0)
    {
        cout << "tFiles\n"; tFiles.print();
        cout << "sFiles\n"; sFiles.print();
    }

    return 0;
}


long readDirR_cntr = 0;
void readDirR(fs::path dir, Files & flist, fs::path rp)
{
    {
        ol::Pushd pushd(dir);
        auto ents = ol::readdir();

        for ( auto f : ents.files() )
        {
            cout << (++readDirR_cntr) << '\r';
            File n;
            n.pth = rp / f.first;
            n.tc = f.second.first;
            n.sz = f.second.second;
            flist.add(n);
        }

        for ( auto d : ents.dirs().names() ) readDirR(d, flist, rp / d);
    }
}

Files loadFrom(fs::path dir)
{
    cout << "loading from [" << dir << "]\n";

    Files f;
    f.dir = dir;

    readDirR_cntr = 0;
    readDirR(dir, f, "");

    return f;
}

void Files::print() const
{
    cout << "path: [" << dir.string() << "]\n";
    for ( auto f : files ) f.print();
}

void File::print() const
{
    cout << "[" << pth.string() << "] " << tc << ' ' << sz
         << " {" << (hashHead.empty() ? "" : hashHead.substr(0, 4)) << "}"
         << " {" << (hashFile.empty() ? "" : hashFile.substr(0, 4)) << "}" << '\n';
}

bool File::same(fs::path dira, File & a, fs::path dirb, File & b)
{
    ///cout << "AAA " << __func__ << '\n';  a.print();    b.print();

    if ( a.sz != b.sz ) return false;

    auto sz = a.sz;

    auto hHash = [sz](fs::path pth) -> string
    {
        const auto HEADSZ = 500ull;
        string head = ol::fileHead2str(pth, HEADSZ);
        if ( sz <= HEADSZ && head.size() < sz ) nevers("underread head");
        return ha::hashHex(head);
    };

    ///if ( a.hashHead.empty() ) a.hashHead = ha::hashHex(ol::fileHead2str(a.pth, 500));
    ///if ( b.hashHead.empty() ) b.hashHead = ha::hashHex(ol::fileHead2str(b.pth, 500));
    if ( a.hashHead.empty() ) a.hashHead = hHash(dira / a.pth);
    if ( b.hashHead.empty() ) b.hashHead = hHash(dirb / b.pth);

    if ( a.hashHead != b.hashHead ) return false;

    auto fHash = [sz](fs::path pth) -> string
    {
        string file = ol::file2str(pth.string());
        if ( file.size() < sz ) nevers("underread file");
        return ha::hashHex(file);
    };

    ///if ( a.hashFile.empty() ) a.hashFile = gfu::fileHash(a.pth.string());
    ///if ( b.hashFile.empty() ) b.hashFile = gfu::fileHash(b.pth.string());
    if ( a.hashFile.empty() ) a.hashFile = fHash(dira / a.pth);
    if ( b.hashFile.empty() ) b.hashFile = fHash(dirb / b.pth);

    ///cout << "full hashes " << a.hashFile << ' ' << b.hashFile << '\n';

    return a.hashFile == b.hashFile;
}

void copy2dest(fs::path dir_dst, fs::path dir_src, fs::path file)
{
    ///cout << "AAA " << dir << ' ' << file.string() << '\n';
    auto destf = dir_dst / file;
    auto destd = destf;
    destd.remove_filename();
    fs::create_directories(destd);
    fs::copy(dir_src / file, destf);
    //cout << "copy: " << file.string() << '\n';
}


void saveData(const Files & files, string filename)
{
    std::ofstream of(filename, std::ios::binary);

    of << files.dir.string() << '\n';
    of << files.files.size() << '\n';

    for ( auto f : files.files )
    {
        of << f.pth.string() << '\n';
        of << f.sz << '\n';
        of << f.tc << '\n';
        of << (f.hashHead.empty() ? "0" : f.hashHead) << '\n';
        of << (f.hashFile.empty() ? "0" : f.hashFile) << '\n';
    }
}

void saveCache(const Files & files)
{
    saveData(files, g_cache_name);
}



Files loadCacheFile(string file)
{
    std::ifstream in(file, std::ios::binary);

    Files fd;

    string s;
    std::getline(in, s);
    fd.dir = s;

    auto load = [&in](auto & x)
    {
        string ss;
        std::getline(in, ss);
        std::istringstream is(ss);
        is >> x;
    };

    int sz = 0;
    load(sz);

    fd.files;

    for_i(sz)
    {
        File f;
        std::getline(in, s); f.pth = s;
        load(f.sz);
        load(f.tc);
        load(f.hashHead);
        load(f.hashFile);

        if ( f.hashHead == "0" ) f.hashHead = "";
        if ( f.hashFile == "0" ) f.hashFile = "";

        fd.add(f);
    }

    return fd;
}

Files loadCache(string file, bool skipRead)
{
    Files fd = loadCacheFile(file);
    if ( skipRead ) return fd;

    Files fr = loadFrom(fd.dir);

    ///cout << "AAA loaded from cache " << fd.files.size() << " loaded from dir " << fr.files.size() << '\n';

    ///cout << "fd\n"; fd.print();
    ///cout << "fr\n"; fr.print();

    std::set<fs::path> fds;
    for ( auto f : fd.files ) fds.insert(f.pth);

    for ( auto f : fr.files )
        if ( fds.find(f.pth) == fds.end() )
            fd.add(f);

    ///cout << "fd NEW\n"; fd.print();

    return fd;
}

int main_snap(ivec<string> args)
{
    if ( args.empty() )
    {
        cout << "snapshot of file in directory\n";
        cout << "it can later be used with 'jadd' without file access\n";
        return 0;
    }

    if ( args.size() != 1 ) throw "Bad arguments";

    string TRG = args[0];

    Files tFiles;

    tFiles = loadFrom(TRG);

    cout << "read " << tFiles.files.size() << " files\n";

    int  cntr2 = 0;
    for ( auto & tf : tFiles.files )
    {
        File::same(tFiles.dir, tf, tFiles.dir, tf);
        cout << (++cntr2) << "/" << tFiles.files.size() << "\r";
    }

    saveData(tFiles, g_snap_name);
}

#include <iostream>
#include <fstream>
#include <map>

#include "olu.h"
#include "gfu.h"
#include "hash.h"

using std::cout;

string g_cache_name = "gf.fadd.log";

struct File
{
    unsigned long long sz = 0;
    string hashHead;
    string hashFile;

    fs::path pth;
    unsigned long long tc = 0;

    void print() const;
    static bool same(File & a, File & b);
};

struct Files
{
    fs::path dir;
    ivec<File> files;
    std::map<unsigned long long, ivec<size_t> > sz2idx;

    void add(File f) { sz2idx[f.sz].push_back(files.size()); files.push_back(f); }
    void print() const;
};

Files loadFrom(string dir);
Files loadCache(string file, bool read);
void copy2dest(fs::path dir, fs::path file);
void saveCache(const Files & files);

int main_fadd(ivec<string> args)
{
    if ( args.empty() )
    {
        cout << "fadd module copies files from SRC to DST\n";
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
    if ( !fs::is_directory(TRG) ) tFiles = loadCache(TRG, skipRead);
    else tFiles = loadFrom(TRG);

    if ( srcFile )
    {
        // sFiles.add(SRC);
        throw "Adding single file is not yet supprted, use dir\n";
    }
    else sFiles = loadFrom(SRC);

    if (0)
    {
        cout << "tFiles\n"; tFiles.print();
        cout << "sFiles\n"; sFiles.print();
    }

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

            if ( File::same(sf, tf) )
            {
                isnew = !true;
                break;
            }
        }

        if ( isnew ) { ++cntr1; copy2dest(DST, sf.pth); }

        cout << (++cntr2) << "/" << sFiles.files.size() << "\r";
    }

    cout << "copied " << cntr1 << " out of " << sFiles.files.size() << " files\n";

    saveCache(tFiles);

    return 0;
}


long readDirR_cntr = 0;
void readDirR(string dir, Files & flist, fs::path rp)
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

Files loadFrom(string dir)
{
    cout << "loading from [" << dir << "]\n";

    Files f;
    f.dir = dir;

    readDirR_cntr = 0;
    readDirR(dir, f, dir);

    return f;
}

void Files::print() const
{
    cout << "path: [" << dir.string() << "]\n";
    for ( auto f : files ) f.print();
}

void File::print() const
{
    cout << "[" << pth.string() << "] " << tc << ' ' << sz << '\n';
}

bool File::same(File & a, File & b)
{
    ///cout << __func__ << '\n';  a.print();    b.print();

    if ( a.sz != b.sz ) return false;

    if ( a.hashHead.empty() ) a.hashHead = ha::hashHex(ol::fileHead2str(a.pth, 500));
    if ( b.hashHead.empty() ) b.hashHead = ha::hashHex(ol::fileHead2str(b.pth, 500));

    if ( a.hashHead != b.hashHead ) return false;

    if ( a.hashFile.empty() ) a.hashFile = gfu::fileHash(a.pth.string());
    if ( b.hashFile.empty() ) b.hashFile = gfu::fileHash(b.pth.string());

    ///cout << "full hashes " << a.hashFile << ' ' << b.hashFile << '\n';

    return a.hashFile == b.hashFile;
}

void copy2dest(fs::path dir, fs::path file)
{
    ///cout << "AAA " << dir << ' ' << file.string() << '\n';
    auto destd = dir / file;
    destd.remove_filename();
    fs::create_directories(destd);
    fs::copy(file, dir / file);
    //cout << "copy: " << file.string() << '\n';
}


void saveCache(const Files & files)
{
    std::ofstream of(g_cache_name, std::ios::binary);

    of << files.dir.string() << '\n';
    of << files.files.size() << '\n';

    for ( auto f : files.files )
    {
        of << f.pth.string() << '\n';
        of << f.sz << '\n';
        of << f.tc << '\n';
        of << f.hashHead << '\n';
        of << f.hashFile << '\n';
    }
}


Files loadCache(string file, bool read)
{
    std::ifstream in(file, std::ios::binary);



    ///string sd; in >>
    never;
}


#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <memory>

#include "olu.h"
#include "gfu.h"
#include "hash.h"

using std::cout;

string g_cache_name = "gf.jadd.log";
string g_snap_name = "gf.snap.log";
string g_same_name = "gf.same.log";

struct File
{
    unsigned long long sz = 0;
    string hashHead;
    string hashFile;

    fs::path pth;
    unsigned long long tc = 0; // time

    void print() const;
    static bool same(fs::path dira, File & a, fs::path dirb, File & b);
    static void fillHash(fs::path dira, File & a, bool headonly);
};

struct DirNode
{
    fs::path dirName, fullName;
    ivec<int> idxs;
    ivec< DirNode * > dirs; // intentional

    unsigned long long sz = 0;
    string hash;

    DirNode() {};
    DirNode(const DirNode &) = delete;
    void operator=(const DirNode &) = delete;
    ~DirNode() { while ( !dirs.empty() ) { delete dirs.back(); dirs.pop_back(); } }
};

struct Files
{
    fs::path dir;
    ivec<File> files;
    std::map<unsigned long long, ivec<int> > sz2idx;

    DirNode * dirTree = nullptr;

    int add(File f)
    {
        int idx = files.size();
        sz2idx[f.sz].push_back(idx);
        files.push_back(f);
        return idx;
    }
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
        cout << "usage: jadd {TRG|[@]cache} SRC DST\n";
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
        throw "Adding single file is not supprted, use dir\n";
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
void readDirR(fs::path dir, Files & flist, fs::path rp, DirNode * dnode = nullptr)
{
    {
        ol::Pushd pushd(dir);
        if (!pushd) nevers("enter dir [" + dir.string() + "] failed");
        auto ents = ol::readdir();

        if (dnode) { dnode->dirName = dir; dnode->fullName /= dir; }

        for ( auto f : ents.files() )
        {
            cout << (++readDirR_cntr) << '\r';
            File n;
            n.pth = rp / f.first;
            n.tc = f.second.first;
            n.sz = f.second.second;
            int idx = flist.add(n);
            if ( dnode ) dnode->idxs.push_back(idx);
        }

        for ( auto d : ents.dirs().names() )
        {
            DirNode * pd = nullptr;
            if (dnode)
            {
                dnode->dirs.push_back(pd = new DirNode);
                pd->fullName = dnode->fullName;
            }
            readDirR(d, flist, rp / d, pd);
        }
    }
}

void loadFrom(fs::path dir, Files & f)
{
    cout << "loading from [" << dir << "]\n";

    f.dir = dir;

    readDirR_cntr = 0;
    readDirR(dir, f, "", f.dirTree);
}

Files loadFrom(fs::path dir)
{
    Files f;
    if (!fs::is_directory(dir)) throw "[" + dir.string() + "] not directory";
    loadFrom(dir, f);
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

void File::fillHash(fs::path dira, File & a, bool headonly)
{
    auto sz = a.sz;

    auto hHash = [sz](fs::path pth) -> string
    {
        const auto HEADSZ = 500ull;
        string head = ol::fileHead2str(pth, HEADSZ);

        if (sz <= HEADSZ && head.size() < sz)
            throw "no access to [" + pth.string() + "]";
        //" underread head sz=" + std::to_string(sz) + " head=" + std::to_string(head.size());

        return ha::hashHex(head);
    };

    if ( a.hashHead.empty() ) a.hashHead = hHash(dira / a.pth);

    if ( headonly ) return;

    auto fHash = [sz](fs::path pth) -> string
    {
        string file = ol::file2str(pth.string());
        if ( file.size() < sz ) nevers("underread file " + pth.string());
        return ha::hashHex(file);
    };

    if ( a.hashFile.empty() ) a.hashFile = fHash(dira / a.pth);
}

bool File::same(fs::path dira, File & a, fs::path dirb, File & b)
{
    //cout << "AAA " << __func__ << '\n';  a.print();    b.print();
    if ( (dira / a.pth) == (dirb / b.pth) ) never;

    /*///
    if ( a.sz != b.sz ) return false;

    auto sz = a.sz;

    auto hHash = [sz](fs::path pth) -> string
    {
        const auto HEADSZ = 500ull;
        string head = ol::fileHead2str(pth, HEADSZ);

        if (sz <= HEADSZ && head.size() < sz)
            throw "no access to [" + pth.string() + "]";
        //" underread head sz=" + std::to_string(sz) + " head=" + std::to_string(head.size());

        return ha::hashHex(head);
    };

    ///if ( a.hashHead.empty() ) a.hashHead = ha::hashHex(ol::fileHead2str(a.pth, 500));
    ///if ( b.hashHead.empty() ) b.hashHead = ha::hashHex(ol::fileHead2str(b.pth, 500));
    if ( a.hashHead.empty() ) a.hashHead = hHash(dira / a.pth);
    if ( b.hashHead.empty() ) b.hashHead = hHash(dirb / b.pth);
    */

    bool headonly = true;
    fillHash(dira, a, headonly);
    fillHash(dirb, b, headonly);

    if ( a.hashHead != b.hashHead ) return false;

    /*///
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
    */

    headonly = !true;
    fillHash(dira, a, headonly);
    fillHash(dirb, b, headonly);

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

    ///cout << "loaded from cache " << fd.files.size() << " loaded from dir " << fr.files.size() << '\n';

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
        cout << "snapshot of files in directory\n";
        cout << "use: snap path - stores to '" << g_snap_name << "'\n";
        cout << "use: snap path file - update/create snap file\n";
        cout << "use: (NI)snap file - update snap file\n";
        cout << "use: (NI)snap @file - update snap file, no read target dir\n";
        cout << "snapshot file can later be used with 'jadd' without file access\n";
        return 0;
    }

    auto snap_name = g_snap_name;
    if ( args.size() == 2 )
    {
        snap_name = args[1];
        args.pop_back();
    }

    if ( args.size() != 1 ) throw "Bad arguments";

    string TRG = args[0];

    {
        /// disable loading big files
        /// this operation is permanent for execution
        ///extern bool g_allow_partial_hash;
        ///g_allow_partial_hash = true;
    }

    Files tFiles = loadFrom(TRG);
    Files cFiles = loadCache(snap_name, true);

    cout << "read " << tFiles.files.size() << " files";
    if (!cFiles.files.empty()) cout << "; from cache " << cFiles.files.size();
    cout << "\n";
    //return 0;

    // update tFile from cache info
    int  cntr1 = 0;
    for ( auto & tf : tFiles.files )
    {
        ++cntr1;
        auto i = cFiles.sz2idx.find(tf.sz);
        if (i == cFiles.sz2idx.end()) continue;
        for ( int j : i->second )
        {
            const auto & cf = cFiles.files[j];
            if (tf.sz != cf.sz) never;
            if (tf.pth != cf.pth) continue;
            if (tf.tc != cf.tc) continue;
            tf.hashFile = cf.hashFile;
            tf.hashHead = cf.hashHead;
        }
        cout << cntr1 << "/" << tFiles.files.size() << "\r";
    }
    cout << '\n';

    // fill hash
    int  cntr2 = 0;
    for ( auto & tf : tFiles.files )
    {
        File::fillHash(tFiles.dir, tf, false);
        cout << (++cntr2) << "/" << tFiles.files.size() << "\r";
    }

    saveData(tFiles, snap_name);
    return 0;
}

void printTree(Files & tf, DirNode & dt, int ind = 0)
{
    string sind(ind, '.');
    cout << sind << "[" << dt.dirName.string() << "] " << dt.fullName.string() << '\n';

    for (auto i : dt.idxs)
    {
        File & f = tf.files[i];
        cout << sind << "[" << f.pth.string() << "]" << '\n';
    }
    for ( auto d : dt.dirs )
    {
        cout << sind << "{} " << d->sz << ' ' << d->hash << '\n';
        printTree(tf, *d, ind + 2);
    }
}

void processTree(Files & tf, DirNode & dt, ivec<DirNode *> & dlist)
{
    string hash_chain;
    ol::ull siz = 0;
    std::set<string> hash_set;

    bool unique = false;

    for (auto i : dt.idxs)
    {
        File & f = tf.files[i];
        //hash_chain += f.pth.string();
        auto fh = f.hashFile;
        if (fh.empty()) unique = true;
        else hash_set.insert(fh);
        siz += f.sz;
    }

    for ( auto d : dt.dirs )
    {
        processTree(tf, *d, dlist);
        if (d->hash.empty()) unique = true;
        else hash_set.insert(d->hash);
        siz += d->sz;
    }

    dt.sz = siz;
    dt.hash.clear(); // not needed;
    if (unique) return;

    for (string h : hash_set) hash_chain += h;
    dt.hash = ha::hashHex(hash_chain);

    dlist.push_back(&dt);
}

int main_same(ivec<string> args)
{
    if ( args.empty() )
    {
        cout << "same - finds same files in directory\n";
        cout << "use: same path\n";
        return 0;
    }

    if ( args.size() != 1 ) throw "Bad arguments";

    string TRG = args[0];

    // different way of loading
    Files tFiles;
    DirNode dirTree;
    tFiles.dirTree = &dirTree;

    ///if (!fs::is_directory(TRG)) throw "[" + TRG + "] not directory";
    loadFrom(TRG, tFiles);

    cout << "read " << tFiles.files.size() << " files\n";

    // Find same files
    std::map<string, std::set<int> > groups;
    int cntr1 = 0, cntr2 = 0;
    for ( auto & tf1 : tFiles.files )
    {
        // get list of files from TRG to compare
        auto tidxs = tFiles.sz2idx[tf1.sz];
        bool aresame = false;
        for ( int i : tidxs )
        {
            auto & tf2 = tFiles.files[i];

            if ( tf1.pth == tf2.pth ) continue;

            if ( File::same(tFiles.dir, tf1, tFiles.dir, tf2) )
            {
                aresame = true;
                groups[tf1.hashFile].insert(i);
            }
        }

        if ( aresame ) { ++cntr1; }

        cout << (cntr1) << "/" << (++cntr2) << "/" << tFiles.files.size() << "\r";
    }

    // Analyse same dirs
    std::map<string, ivec<DirNode *> > dgrps;
    {
        auto & dt = *tFiles.dirTree;
        //cout << "Dirs: [" << dt.dirName << "] " << dt.files.size() << ' ' << dt.dirs.size() << '\n';
        ivec<DirNode *> dlist;
        processTree(tFiles, dt, dlist);
        auto tmp = dgrps;
        for (auto p : dlist) tmp[p->hash].push_back(p);
        for (auto x : tmp) if (x.second.size() > 1) dgrps.insert(x);
        //printTree(tFiles, dt);

        // FIXME 1 exclude files from same directories
        // FIXME 2 exclude sub-dirs from same dirs
    }


    // Output
    if ( !groups.empty() )
    {
        cout << "found same files : " << groups.size() << " groups\n";

        std::ofstream of(g_same_name, std::ios::binary);

        of << "DIR: " << tFiles.dir.string() << '\n';
        of << "Files: " << tFiles.files.size() << '\n';

        of << "\nList of dir groups: " << dgrps.size() << '\n';
        for (auto & de2 : dgrps)
        {
            auto & de = de2.second;
            if (de.size() < 2) never;
            of << '\n' << de.size() << ' '
               << ( de[0]->sz ) << '\n';

            for (auto p : de)
            {
                ///const auto& f = tFiles.files[i];
                of << p->fullName.string() << '\n';
            }
        }

        of << "\nList of file groups: " << groups.size() << '\n';
        for ( auto & ge : groups )
        {
            const string & hashF = ge.first;
            const std::set<int> & ilist = ge.second;

            if ( ilist.empty() ) never;

            of << '\n' << ilist.size() << ' '
               << tFiles.files[*ilist.begin()].sz << '\n';

            for ( auto i : ilist )
            {
                const auto & f = tFiles.files[i];
                of << f.pth.string() << '\n';
                //of << f.sz << '\n';
                //of << f.tc << '\n';
                //of << (f.hashHead.empty() ? "0" : f.hashHead) << '\n';
                //of << (f.hashFile.empty() ? "0" : f.hashFile) << '\n';
            }
        }
    }

    saveCache(tFiles);
    return 0;
}


#include <iostream>
#include <fstream>
#include <filesystem>

#include "gf.h"
#include "gfu.h"
#include "olu.h"
#include "hash.h"

using std::cout;
namespace fs = std::filesystem;

using vs = ivec<string>;

/*
sync [.] - needs .gf, syncs all files and down
sync dir - enter dir, same
sync file - needs .gf, find gf entry for file, syncs only 1 file
co @path - non-recirsive
co file - no .gf at src, no .gf here or no entry in .gf
co path - no .gf here, no .gf at src, enpty dir
co path - path is upper - disable
co path - path is not upper
clean - erase entry from .gf, if .gf empty - remove
*/

namespace sync
{

struct EntryMap
{
    string src_name, dst_name;
    vs exts; // left-right order
    EntryMap(string srcfile);
    bool packed() const { return !exts.empty(); }
};

struct Entry
{
    bool absent = true;

    string src_path, dst_path;
    ol::ull src_time;
    string src_hash, dst_hash;
    string ent_path;
    ol::ull ent_time;

    bool operator!() const { return absent; }
    static ivec<Entry> load_all();
    Entry() {}
    static Entry src(string file);
    static Entry dst(string file);
    Entry(fs::path file);
    static Entry make(string srcfile);
    void write() const;

    static fs::path src2entry(fs::path srcfile);
    EntryMap entryMap() const;
};

struct Status
{
    enum Typ {Undef = 0, Absent, Insync, Conflict, Modified, Lapsed} typ = Undef;
    Status() {}
    Status(Entry e);
    Status(Typ t) : typ(t) {}
    bool operator==(Status x) const { return typ == x.typ; }
    string str() const { return string("[") + "?AICML"[typ] + "]"; }
};

void init();

void sy_file(Entry ent);
void sy_file(string file);
void sy_dir_final();
void sy_dir_final(string file);
void sy_dir_rec(string file);

void co_file(string file);
void co_dir_final(string file);
void co_dir_rec(string file);

void st_file(Entry e);
void st_file(string file);
void st_dir_final(string file);
void st_dir_rec(string file);

void cl_file(string file);
void cl_dir_final(string file);
void cl_dir_rec(string file);

bool is_dotgf();
void make_dotgf();
string file_here(fs::path file);
void ci_file(Entry ent, EntryMap em);

// globals
fs::path g_dotgf; // ".gf"
bool g_recursive_mode = false;
fs::path g_cwd;

} // sync

sync::Entry::Entry(fs::path file)
{
    ent_time = ol::filetime(file);
    std::ifstream in(file);
    string s;
    in
            >> s >> src_path >> s >> dst_path
            >> s >> src_time
            >> s >> src_hash >> s >> dst_hash;
    if (!in) return;
    ///throw "corrupted entry " + file_here(file);
    absent = false;
    ent_path = file.string();
}

fs::path sync::Entry::src2entry(fs::path fpath)
{
    string spath = fpath.string();
    if (spath.find_first_of(" \n\r\t") != string::npos)
        throw "path [" + spath + "] has spaces";

    auto par = fpath.parent_path();
    auto fn = fpath.filename();
    fn += ".e";
    return g_dotgf / fn;
}

sync::EntryMap sync::Entry::entryMap() const
{
    fs::path srcpath(src_path);
    return EntryMap(srcpath.filename().string());
}

sync::Entry sync::Entry::src(string file)
{
    return Entry(src2entry(file));
}

sync::Entry sync::Entry::dst(string file)
{
    ivec<Entry> ents = Entry::load_all();
    for (auto e : ents) if (e.dst_path == file) return e;
    return Entry();
}

sync::Entry sync::Entry::make(string srcfile)
{
    auto ent_filename = src2entry(srcfile);
    Entry e;

    e.ent_path = ent_filename.string();
    e.src_path = srcfile;
    e.dst_path = e.entryMap().dst_name;
    e.src_time = ol::filetime(srcfile);
    e.dst_hash = "0";
    e.ent_time = 0;
    e.src_hash = gfu::fileHash(srcfile);

    return e;
}

void sync::Entry::write() const
{
    if (src_path.empty()) nevers("src_path");
    if (dst_path.empty()) nevers("dst_path");
    if (src_hash.empty()) nevers("src_hash");
    if (dst_hash.empty()) nevers("dst_hash");
    if (ent_path.empty()) nevers("ent_path");

    auto fname = src2entry(src_path); // this may fail
    if (!is_dotgf()) make_dotgf(); // this is the last point to create .gf
    std::ofstream of(fname);
    if (!of) never;

    of << "src_path " << src_path << '\n';
    of << "dst_path " << dst_path << '\n';
    of << "src_time " << src_time << '\n';
    of << "src_hash " << src_hash << '\n';
    of << "dst_hash " << dst_hash << '\n';
    of << "ent_path " << ent_path << '\n';
    of << "ent_time " << ent_time << '\n';

    if ( !of ) never;
}

sync::EntryMap::EntryMap(string srcfilename)
{
    src_name = srcfilename;
    static vs valid_exts {{".bz2", ".g", ".gfc", ".bzc", ".fcl"}};

    auto f = srcfilename;
    while (1)
    {
        string ext;
        string fncut;
        for (auto e : valid_exts)
        {
            if (ol::endsWith(f, e, fncut))
            {
                ext = e;
                f = fncut;
                break;
            }
        }
        if (ext.empty()) break;
        exts.push_back(ext);
    }

    dst_name = f;
    exts.reverse();
}


ivec<sync::Entry> sync::Entry::load_all()
{
    ivec<Entry> r;

    vs ents;
    {
        ol::Pushd pushd(g_dotgf);
        if (!pushd) return r;
        ents = ol::readdir().files().names();
    }

    for (auto f : ents)
        r.emplace_back(Entry(g_dotgf / f));

    return r;
}

void sync::init()
{
    g_dotgf = ".gf";
}

bool sync::is_dotgf()
{
    return fs::is_directory(g_dotgf);
}

void sync::make_dotgf()
{
    cout << ("SYNC create .gf in [" + g_cwd.string() + "]") << '\n';
    fs::create_directories(g_dotgf);
}


string sync::file_here(fs::path file)
{
    fs::path f = g_cwd;
    if ( f == "." ) f = "";
    if ( !f.empty() ) f = (fs::path(f) / file).string();
    else f = file;
    return f.string();
}

sync::Status::Status(Entry e)
{
    if (!fs::exists(e.dst_path)) { typ = Absent; return; }

    // Insync, Conflict, Modified, Lapsed
    // Src < Ent < Dst => Modified
    // Src < Ent > Dst => Insync
    // Src > Ent < Dst => Conflict
    // Src > Ent > Dst => Lapsed

    auto tsrc = ol::filetime(e.src_path);
    auto tdst = ol::filetime(e.dst_path);
    auto tent = ol::filetime(e.ent_path);

    ///cout << "src ent dst\n" << tsrc << '\n' << tent << '\n' << tdst << '\n';

    if (tsrc < tent)
    {
        if (tent < tdst) typ = Modified;
        else typ = Insync;
    }
    else
    {
        if (tent < tdst) typ = Conflict;
        else typ = Lapsed;
    }
}


int main_sync(vs args, int sync_co_st) // 1234
{
    sync::init();

    string dof; // dir or file
    if ( args.empty() ) {}
    else if ( args.size() == 1 ) dof = args[0];
    else throw "too many params";

    bool isdir = true;
    bool isrec = true;

    if (dof[0] == '@')
    {
        dof = dof.substr(1);
        isrec = false;
    }

    if ( dof.empty() ) {}
    else
    {
        if ( !fs::exists(dof) ) throw "no " + dof;

        if ( fs::is_regular_file(dof) ) isdir = false;
        else if ( fs::is_directory(dof) ) {}
        else throw "bad path " + dof;
    }

    sync::g_recursive_mode = isrec && isdir;

    // ///cout << "SYNC d f isd, isr [" << dir << "] [" << file << "] " << isdir << ' ' << isrec << '\n';

    if ( sync_co_st < 1 || sync_co_st > 4 ) never;

    if ( sync_co_st == 1 )
    {
        if (!isdir) sync::sy_file(dof);
        else if (isrec) sync::sy_dir_rec(dof);
        else sync::sy_dir_final(dof);
    }

    if ( sync_co_st == 2 )
    {
        if ( !isdir ) sync::co_file(dof);
        else if ( isrec ) sync::co_dir_rec(dof);
        else sync::co_dir_final(dof);
    }

    if ( sync_co_st == 3 )
    {
        if ( !isdir ) sync::st_file(dof);
        else if ( isrec ) sync::st_dir_rec(dof);
        else sync::st_dir_final(dof);
    }

    if ( sync_co_st == 4 )
    {
        if ( !isdir ) sync::cl_file(dof);
        else if ( isrec ) sync::cl_dir_rec(dof);
        else sync::cl_dir_final(dof);
    }

    return 0;
}


void sync::sy_file(Entry ent)
{
    if ( !ent ) never;

    Status st(ent);

    if (st == Status::Insync) return;

    if (st == Status::Conflict)
    {
        cout << st.str() << ' ' << file_here(ent.dst_path) << '\n';
        return;
    }

    if (st == Status::Lapsed) fs::remove_all(ent.dst_path);

    EntryMap em = ent.entryMap();

    if (st == Status::Absent || st == Status::Lapsed)
    {
        cout << "$ => " << st.str() << " " << file_here(ent.dst_path) << '\n';

        {
            string fbody = ol::file2str(ent.src_path);
            ent.src_hash = ha::hashHex(fbody);
            std::ofstream of(em.src_name, std::ios::binary);
            of << fbody;
        }

        ent.dst_hash = ent.src_hash;
        if (em.packed())
        {
            main_pack({{em.src_name}}, false);

            if (!fs::exists(ent.dst_path)) never;
            string dbody = ol::file2str(ent.dst_path);
            ent.src_hash = ha::hashHex(dbody);
        }

        ent.src_time = ol::filetime(ent.src_path);
        ent.write();
        return;
    }

    if (st == Status::Modified)
    {
        cout << "$ <= " << st.str() << ' ' << file_here(ent.dst_path) << '\n';
        ci_file(ent, em);
        return;
    }

    never;
}

void sync::sy_file(string file)
{
    Entry ent = Entry::dst(file);
    if ( !ent ) throw "no entry for " + file_here(file);
    sy_file(ent);
}

void sync::sy_dir_final()
{
    if ( !is_dotgf() )
    {
        if ( g_recursive_mode ) return; // no error
        throw "not gf checkout " + g_cwd.string();
    }

    auto ents = Entry::load_all();
    for (auto e : ents) sy_file(e);
}

void sync::sy_dir_final(string dir)
{
    ol::Pushd pushd(dir, g_cwd);
    sy_dir_final();
}

void sync::sy_dir_rec(string dir)
{
    ol::Pushd pushd(dir, g_cwd);

    sy_dir_final();

    // for all directories
    // if not .gf entry
    // enter dir + dir_final

    auto dirs = ol::readdir().dirs().names();
    for ( const auto & name : dirs )
    {
        Entry ent = Entry::dst(name);
        if ( !!ent ) continue; // this is checkout
        sy_dir_final(name);
    }
}

void sync::co_file(string file)
{
    {
        Entry ent = Entry::src(file);
        if (!!ent) throw "entry exists " + file_here(file);
    }

    {
        Entry e = Entry::make(file);

        const auto & dst = e.dst_path;

        if (fs::exists(dst)) // fail with different messages
        {
            if (fs::equivalent(e.src_path, dst))
                throw "cannot co to itself " + file_here(file);
            throw "cannot co - exist " + file_here(dst);
        }

        e.write();
    }

    Entry ent = Entry::src(file);
    if ( !ent ) never;
    sy_file(ent);
}

void sync::co_dir_final(string file) { never; }
void sync::co_dir_rec(string file) { never; }


void sync::st_file(Entry e)
{
    Status s(e);
    if (g_recursive_mode && s == Status::Insync) return;

    cout << s.str() << ' ' << (g_cwd / e.dst_path).string() << '\n';
}

void sync::st_file(string file)
{
    auto e = Entry::dst(file);
    if (!e)
    {
        cout << Status().str() << " " << file << '\n';
        return;
    }
    st_file(e);
}

void sync::st_dir_final(string dir)
{
    ol::Pushd pushd(dir, g_cwd);

    if (!is_dotgf())
    {
        if (g_recursive_mode) return;
        throw "not gf dir [" + dir + "]";
    }

    auto ents = Entry::load_all();
    ol::Msul dofs = ol::readdir();
    for (auto e : ents)
    {
        st_file(e);
        dofs.erase(e.dst_path);
    }

    for (auto d : dofs.dirs().names())
    {
        fs::path pd = d;
        if (g_dotgf == pd) continue;
        if (fs::exists(pd / g_dotgf)) continue;
        cout << Status().str() << " " << d << '\n';
    }

    for (auto file : dofs.files().names())
    {
        cout << Status().str() << " " << file << '\n';
    }
}

void sync::st_dir_rec(string dir)
{
    st_dir_final(dir);

    ol::Pushd pushd(dir, g_cwd);

    ol::Msul dofs = ol::readdir();

    for (auto d : dofs.dirs().names())
    {
        fs::path pdir = d;
        if (g_dotgf == pdir) continue;
        st_dir_rec(d);
    }
}

void sync::cl_file(string file) { never; }
void sync::cl_dir_final(string file) { never; }
void sync::cl_dir_rec(string file) { never; }

void pack_bzc(string & name)
{
    main_pack({ {name} }, true);
    name += ".bzc";
    if (!fs::exists(name)) never;
}

void sync::ci_file(Entry ent, EntryMap em)
{
    auto curname = em.dst_name;
    for (auto ext : em.exts)
    {
        if (0) {}
        if (ext == ".bzc") pack_bzc(curname);
        else nevers("file type [" + ext + "] not recognized");
    }

    if (!fs::exists(curname)) never;

    fs::rename(curname, ent.src_path);

    sy_file(ent);
}

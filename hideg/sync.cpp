
#include <iostream>
#include <fstream>
#include <filesystem>

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

struct Entry
{
    bool absent = true;

    string src_path, dst_path;
    string src_time, ent_time;
    string src_hash, dst_hash;

    bool operator!() const { return absent; }
    ///enum Typ { by_dst, by_src, load_file };
    ///Entry(Typ typ, string file);
    static ivec<Entry> load_all();
    Entry() {}
    static Entry src(string file);
    static Entry dst(string file);
    Entry(fs::path file);
};

void init();

void sy_file(string file);
void sy_dir_final();
void sy_dir_final(string file);
void sy_dir_rec(string file);

void co_file(string file);
void co_dir_final(string file);
void co_dir_rec(string file);

void st_file(string file);
void st_dir_final(string file);
void st_dir_rec(string file);

void cl_file(string file);
void cl_dir_final(string file);
void cl_dir_rec(string file);

bool is_dotgf();
void make_dotgf();
string file_here(fs::path file);

// globals
fs::path dotgf; // ".gf"
bool recursive_mode = true;
fs::path g_cwd;

} // sync

sync::Entry::Entry(fs::path file)
{
    ent_time = std::to_string(ol::filetime(file));
    std::ifstream in(file);
    string s;
    in
            >> s >> src_path >> s >> dst_path
            >> s >> src_time
            >> s >> src_hash >> s >> dst_hash;
    if (!in) return;
    ///throw "corrupted entry " + file_here(file);
    absent = false;
}

sync::Entry sync::Entry::src(string file)
{
    fs::path fpath = file;
    auto par = fpath.parent_path();
    auto fn = fpath.filename();
    fn += ".e";
    return Entry(dotgf / fn);
}

sync::Entry sync::Entry::dst(string file)
{
    ivec<Entry> ents = Entry::load_all();
    for (auto e : ents) if (e.dst_path == file) return e;
    return Entry();
}

/*///
sync::Entry::Entry(Typ typ, string file)
{
    if (0) {}
    else if ( typ == by_src )
    {
        fs::path fpath = file;
        auto par = fpath.parent_path();
        auto fn = fpath.filename();
        never;
    }

    else if ( typ == by_dst )
    {
        ivec<Entry> ents = Entry::load_all();
        for ( auto e : ents )
        {
            if ( e.dst_path == file )
            {
                *this = e;
                return;
            }
        }
        absent = true;
    }

    else if (typ == load_file)
    {
        std::ifstream in(file);
        string s;
        in
                >> s >> src_path >> s >> dst_path
                >> s >> src_time
                >> s >> src_hash >> s >> dst_hash;
        if (!in) throw "corrupted entry " + file_here(file);
    }

    else never;

}
*/

ivec<sync::Entry> sync::Entry::load_all()
{
    ivec<Entry> r;

    ol::Pushd pushd(dotgf);
    if (!pushd) return r;

    auto ents = ol::readdir().files().names();

    for (auto f : ents)
        r.emplace_back(Entry(f));

    return r;
}

void sync::init()
{
    dotgf = ".gf";
}

bool sync::is_dotgf()
{
    return fs::is_directory(dotgf);
}

void sync::make_dotgf()
{
    cout << ("SYNC create .gf in [" + g_cwd.string() + "]") << '\n';
    fs::create_directories(dotgf);
}


string sync::file_here(fs::path file)
{
    fs::path f = g_cwd;
    if ( f == "." ) f = "";
    if ( !f.empty() ) f = (fs::path(f) / file).string();
    else f = file;
    return f.string();
}


int main_sync(vs args, int sync_co_st) // 1234
{
    sync::init();

    string dir_or_file;
    if ( args.empty() ) {}
    else if ( args.size() == 1 ) dir_or_file = args[0];
    else throw "too many params";

    string dir, file; // FIXME remove
    bool isdir = true;
    bool isrec = true;

    if (dir_or_file[0] == '@')
    {
        dir_or_file = dir_or_file.substr(1);
        isrec = false;
    }

    if ( dir_or_file.empty() ) {}
    else
    {
        if ( !fs::exists(dir_or_file) ) throw "no " + dir_or_file;

        if ( fs::is_regular_file(dir_or_file) ) isdir = false;
        else if ( fs::is_directory(dir_or_file) ) {}
        else throw "bad path " + dir_or_file;

        if ( isdir ) dir = dir_or_file;
        else file = dir_or_file;
    }

    sync::recursive_mode = isrec;

    cout << "SYNC d f isd, isr [" << dir << "] [" << file << "] " << isdir << ' ' << isrec << '\n';

    if ( sync_co_st < 1 || sync_co_st > 4 ) never;

    if ( sync_co_st == 1 )
    {
        if ( !isdir ) sync::sy_file(file);
        else if ( isrec ) sync::sy_dir_rec(dir);
        else sync::sy_dir_final(dir);
    }

    if ( sync_co_st == 2 )
    {
        if ( !isdir ) sync::co_file(file);
        else if ( isrec ) sync::co_dir_rec(dir);
        else sync::co_dir_final(dir);
    }

    if ( sync_co_st == 3 )
    {
        if ( !isdir ) sync::st_file(file);
        else if ( isrec ) sync::st_dir_rec(dir);
        else sync::st_dir_final(dir);
    }

    if ( sync_co_st == 4 )
    {
        if ( !isdir ) sync::cl_file(file);
        else if ( isrec ) sync::cl_dir_rec(dir);
        else sync::cl_dir_final(dir);
    }

    return 0;
}


void sync::sy_file(string file)
{
    Entry ent = Entry::dst(file);
    if ( !ent ) throw "no entry for " + file_here(file);
    never;
}

void sync::sy_dir_final()
{
    if ( !is_dotgf() )
    {
        if ( recursive_mode ) return; // no error
        throw "not gf checkout " + g_cwd.string();
    }

    never;
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
    if ( !is_dotgf() ) make_dotgf();
    {
        Entry entd = Entry::src(file);
        if (!!entd) throw "entry exists " + file_here(file);
    }

    // make_entry() if src==dst throw
    never;
}

void sync::co_dir_final(string file) { never; }
void sync::co_dir_rec(string file) { never; }

void sync::st_file(string file) { never; }
void sync::st_dir_final(string file) { never; }
void sync::st_dir_rec(string file) { never; }

void sync::cl_file(string file) { never; }
void sync::cl_dir_final(string file) { never; }
void sync::cl_dir_rec(string file) { never; }

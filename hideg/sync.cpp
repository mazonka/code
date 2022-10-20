
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
    bool ok = true;
    bool operator!() const { return !ok; }
    enum Typ { by_dst, by_src };
    Entry(Typ typ, string file);
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
string file_here(string file);

// globals
fs::path dotgf; // ".gf"
bool recursive_mode = true;
fs::path g_cwd;

} // sync


sync::Entry::Entry(Typ typ, string file)
{
    if ( typ == by_src )
    {
        never;
    }

    if ( typ == by_dst )
    {
        never;
    }

    never;

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
    fs::create_directories(dotgf);
}


string sync::file_here(string file)
{
    string f = g_cwd.string();
    if ( f == "." ) f = "";
    if ( !f.empty() ) f = (fs::path(f) / file).string();
    else f = file;
    return f;
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
}


void sync::sy_file(string file)
{
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
        Entry ent(Entry::by_dst, name);
        if ( !!ent ) continue; // this is checkout
        sy_dir_final(name);
    }
}

void sync::co_file(string file)
{
    if ( !is_dotgf() ) make_dotgf();
    {
        Entry entd(Entry::by_src, file);
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


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
void sy_file(string file);
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
} // sync

int main_sync(vs args, int sync_co_st) // 1234
{
    string dir_or_file;
    if ( args.empty() ) {}
    else if ( args.size() == 1 ) dir_or_file = args[0];
    else throw "too many params";

    string dir, file; // FIXME remove
    bool isdir = true;
    bool isrec = true;

    if ( dir_or_file.empty() ) {}
    else
    {
        if ( dir_or_file[0] == '@' )
        {
            dir_or_file = dir_or_file.substr(1);
            isrec = false;
        }

        if ( !fs::exists(dir_or_file) ) throw "no " + dir_or_file;

        if ( fs::is_regular_file(dir_or_file) ) isdir = false;
        else if ( fs::is_directory(dir_or_file) ) {}
        else throw "bad path " + dir_or_file;

        if ( isdir ) dir = dir_or_file;
        else file = dir_or_file;
    }

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

    never;
}


void sync::sy_file(string file)
{
    never;
}

void sync::sy_dir_final(string dir)
{
    if ( !fs::is_directory(dir) )
        throw "not gf checkout" + (dir.empty() ? "" : " " + dir);
    never;
}

void sync::sy_dir_rec(string dir)
{
    sy_dir_final(dir);

    // for all directories
    // if not .gf entry
    // enter dir + dir_final
    never;
}

void sync::co_file(string file)
{
    //if( !is_dotgf() ) make_dotgf();
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

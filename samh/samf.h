#pragma once

#include <map>

#include "olc.h"
#include "os_filesys.h"
#include "timer.h"
#include "ma_skc.h"
#include "os_mtime.h"

#define VERSION "19.1009.2"

struct QfHash
{
    string q; // 100-hash
    string f; // full hash

    ///string str() const;

    bool operator<(const QfHash & x) const
    {
        if ( q < x.q ) return true;
        if ( x.q < q ) return false;
        return f < x.f;
    }

    bool operator==(const QfHash & x) const
    {
        return !( *this < x || x < *this );
    }
};

namespace sam
{

const ol::ull SZMAX = 10ull * 1000 * 1000 * 1000;
const ol::ull PRN = 1ull * 1000 * 100;

struct File
{
    string dname;
    string fname;
    time_t mtime;
    ol::ull size;
    bool operator<(const File & x) const
    {
        if ( dname < x.dname ) return true;
        if ( dname > x.dname ) return false;
        if ( fname < x.fname ) return true;
        if ( fname > x.fname ) return false;
        if ( size < x.size ) return true;
        if ( size > x.size ) return false;
        return mtime < x.mtime;
    }

    string name() const { return dname + fname; }
    string sname() const { return fname + '@' + ol::tos(size); };
};

//using msu = std::map<string, ull>;
using mfu = std::map<File, ol::ull>;
using mfs = std::map<File, string>;

struct SH
{
    string ssize;
    string hash;
    bool operator<(const SH & x) const
    {
        if ( ssize < x.ssize ) return true;
        if ( ssize > x.ssize ) return false;
        return hash < x.hash;
    }
};

using vfile = std::vector<File>;
using mshvf = std::map<SH, vfile>;

string gethash(string f, ol::ull sz, bool three);
string gethash_cache(File f, ol::ull sz, bool three);

mfu getListOfFiles(os::Path p, bool incDot);
mfu getListOfFilesR(os::Path p, bool incDot);
void prn_buckets(mshvf & buckets);
void save_buckets(string name, mshvf & newb);

string qhash(string f);
string fhash(string f);

string qhash(sam::File f);
string fhash(sam::File f);

string qhcache(std::pair<const sam::File, QfHash> & a);
string fhcache(std::pair<const sam::File, QfHash> & a);

} //sam


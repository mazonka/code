#pragma once

#include "ol.h"
#include "os_filesys.h"
#include "timer.h"
#include "ma_skc.h"
#include "os_mtime.h"

namespace sam
{

const ull SZMAX = 10ull * 1000 * 1000 * 1000;
const ull PRN = 1ull * 1000 * 100;

struct File
{
    string dname;
    string fname;
    time_t mtime;
    ull size;
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
    string sname() const { return fname + '@' + tos(size); };
};

//using msu = std::map<string, ull>;
using mfu = std::map<File, ull>;
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

string gethash(string f, ull sz, bool three);
string gethash_cache(File f, ull sz, bool three);

mfu getListOfFiles(os::Path p, bool incDot);
void prn_buckets(mshvf & buckets);
void save_buckets(string name, mshvf & newb);

} //sam


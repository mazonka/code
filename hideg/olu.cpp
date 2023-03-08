#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

#include "olu.h"

namespace fs = std::filesystem;
using std::cout;

ol::Warn_t ol::warn;

bool ol::endsWith(string s, string fx)
{
    auto sz = s.size();
    auto fz = fx.size();
    return sz >= fz && 0 == s.compare(sz - fz, fz, fx);
};

bool ol::endsWith(string s, string fx, string & cut)
{
    bool r = endsWith(s, fx);
    if ( r ) cut = s.substr(0, s.size() - fx.size());
    return r;
}

string ol::file2str(const fs::path & file)
{
    if ( !fs::exists(file) ) return "";
    if ( !fs::is_regular_file(file) ) throw "file [" + file.string() + "] is not file";

    const size_t MAX_FILE_SIZE = size_t(1024) * 1024 * 20000; // 20Gb
    std::ifstream in(file, std::ios::binary);

    if ( !in )
        return "";

    string r;

    in.seekg(0, std::ios::end);

    size_t sz = size_t(in.tellg());

    if ( sz > MAX_FILE_SIZE )
    {
        throw "File [" + file.string() + "] too big: sz=" + std::to_string(sz)
        + " max=" + std::to_string(MAX_FILE_SIZE);
    }

    r.reserve( sz );
    in.seekg(0, std::ios::beg);

    r.assign( std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() );

    return r;
}

string ol::fileHead2str(const fs::path & file, size_t szi)
{
    std::ifstream is(file, std::ios::binary);

    const int BSZ = 10000;
    if ( szi >= BSZ ) nevers("fileHead2str: Head is too big");

    char buf[BSZ + 1];

    is.read(buf, szi);

    auto sz = is.gcount();
    string x(buf, sz);

    return x;
}

bool ol::delfile(string fn)
{
    if ( !fs::exists(fn) ) return false;
    fs::remove(fn);
    return !fs::exists(fn);
}

ol::Msul ol::readdir()
{
    Msul r;
    auto cdir = fs::directory_iterator(".");
    for ( auto const & de : cdir )
    {
        string nm = de.path().filename().string();
        unsigned long long tc = de.last_write_time().time_since_epoch().count();
        long long sz = -1L;
        if (!de.is_directory()) sz = (long long)de.file_size();
        r[nm] = std::pair<unsigned long long, long long>(tc, sz);
    }

    return r;
}

ol::Msul ol::Msul::dirs() const
{
    Msul r;
    for ( const auto & e : *this ) if (e.second.second < 0) r.insert(e);
    return r;
}

ol::Msul ol::Msul::files() const
{
    Msul r;
    for (const auto & e : *this) if (e.second.second >= 0) r.insert(e);
    return r;
}

ol::vs ol::Msul::names() const
{
    vs r;
    for ( const auto & e : *this ) r.push_back(e.first);
    return r;
}

unsigned long long ol::filetime(fs::path dof)
{
    //return 1;

    if ( fs::is_regular_file(dof) )
    {
        auto file = dof;
        std::error_code err;
        auto lwt = 1ull * fs::last_write_time(file, err).time_since_epoch().count();
        bool ok = !err.default_error_condition();
        if ( !ok ) return 0;
        return lwt;
    }

    if ( !fs::is_directory(dof) ) return 0;

    ol::ull tim = 0;
    Pushd pd(dof);
    auto dir = readdir();

    for ( auto fn : dir.names() )
    {
        auto etim = filetime(fn);
        if ( etim > tim ) tim = etim;
    }

    return tim;
}

bool ol::replaceAll(string & s, const string & r, const string & to)
{
    bool found = false;
    while (1)
    {
        size_t i = s.find(r);
        if ( i == string::npos ) return found;
        s.replace(i, r.size(), to);
        found = true;
    }
    throw 0; // never
}

template<class T>
ol::Warn_t & operator<<(ol::Warn_t & os, T x)
{
    cout << x;
    std::ofstream("gf.warn.log", std::ios::binary) << x;
    return os;
}

template ol::Warn_t & operator<< (ol::Warn_t & os, int x);
template ol::Warn_t & operator<< (ol::Warn_t & os, string x);
template ol::Warn_t & operator<< (ol::Warn_t & os, const string & x);
template ol::Warn_t & operator<< <const char *>(ol::Warn_t & os, const char * x);


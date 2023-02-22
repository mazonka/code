#pragma once

#include <map>
#include <filesystem>

#include "ccrun.h"

namespace fs = std::filesystem;

namespace ol
{
template<class T> struct ivec : std::vector<T>
{
    using Base = std::vector<T>;
    int size() const { return (int)Base::size(); }
    void operator+=(const T & b) { Base::push_back(b); }
    ivec operator+(const T & b) { auto r(*this); r += b; return r; }
    void operator+=(const ivec<T> & x) { Base::insert(Base::end(), x.begin(), x.end()); }
    ivec operator+(const ivec<T> & x) { auto r(*this); r += x; return r; }
    void erase(int i) { Base::erase(Base::begin() + i); }
    void reverse() { std::reverse(Base::begin(), Base::end()); }
    T add(const T & sep) const { T r {}; for ( const T & x : *this) { r += sep; r += x;} return r; }
    string str(const string & sep) const { string r; for ( const T & x : *this) { r += sep; r += std::to_string(x);} return r; }
};

using vs = ivec<string>;

inline bool replaceAll(string & s, const string & r, const string & to)
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

inline bool endsWith(string s, string fx)
{
    auto sz = s.size();
    auto fz = fx.size();
    return sz >= fz && 0 == s.compare(sz - fz, fz, fx);
};

inline bool endsWith(string s, string fx, string & cut)
{
    bool r = endsWith(s, fx);
    if ( r ) cut = s.substr(0, s.size() - fx.size());
    return r;
}

inline string file2str(const fs::path & file)
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

} // ol

namespace fsut
{

struct Pushd
{
    fs::path cwd;
    fs::path * store = nullptr;
    bool bad = false;
    ~Pushd()
    {
        if (!cwd.empty() && cwd != ".") fs::current_path(cwd);
        if (store) *store = cwd;
        store = nullptr;
    }
    Pushd(fs::path d, fs::path & newp) : Pushd(d) { newp = cwd / d; store = &newp;  }
    Pushd(fs::path d)
    {
        if (!d.empty() && d != ".") cwd = fs::current_path();
        std::error_code ec;
        fs::current_path(d, ec);
        bad = !!ec.default_error_condition();
    }
    bool operator!() const { return bad; }
};

typedef std::map<std::string, std::pair<unsigned long long, long long> > msul;
struct Msul : msul
{
    Msul dirs() const;
    Msul files() const;
    ol::vs names() const;
};

inline Msul Msul::dirs() const
{
    Msul r;
    for ( const auto & e : *this ) if (e.second.second < 0) r.insert(e);
    return r;
}

inline Msul Msul::files() const
{
    Msul r;
    for (const auto & e : *this) if (e.second.second >= 0) r.insert(e);
    return r;
}

inline ol::vs Msul::names() const
{
    ol::vs r;
    for ( const auto & e : *this ) r.push_back(e.first);
    return r;
}

//string file2str(const fs::path & file);
//bool delfile(string file);
//unsigned long long filetime(fs::path file);
inline Msul readdir(bool u8 = false)
{
    Msul r;
    auto cdir = fs::directory_iterator(".");
    for ( auto const & de : cdir )
    {
        auto anm = de.path().filename();
        string nm = u8 ? anm.u8string() : anm.string();
        unsigned long long tc = de.last_write_time().time_since_epoch().count();
        long long sz = -1L;
        if (!de.is_directory()) sz = (long long)de.file_size();
        r[nm] = std::pair<unsigned long long, long long>(tc, sz);
    }

    return r;
}

} // fsut

XXX

USE FILE IN CCRUN

#pragma once

#include <map>
#include <filesystem>

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

typedef std::map<std::string, std::pair<unsigned long long, long> > msul;
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
inline Msul readdir()
{
    Msul r;
    auto cdir = fs::directory_iterator(".");
    for ( auto const & de : cdir )
    {
        string nm = de.path().filename().string();
        unsigned long long tc = de.last_write_time().time_since_epoch().count();
        long sz = -1L;
        if (!de.is_directory()) sz = (long)de.file_size();
        r[nm] = std::pair<unsigned long long, long>(tc, sz);
    }

    return r;
}

} // fsut

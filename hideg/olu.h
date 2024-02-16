#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <map>
#include <algorithm>

using std::string;
namespace fs = std::filesystem;

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

    ivec(const Base & x) : Base(x) {}
    ivec() : Base() {}
};

#ifndef for_i
#define for_i(x) for(int i=0; i<(x); ++i)
#define for_ir(x) for(int i=(x); i>=0; --i)
#define for_(i,x) for(int i=0; i<(x); ++i)
#endif

#ifndef never
#define nevers(x) (throw std::string("err at ")+(__FILE__)+":"+std::to_string(__LINE__) + " ["+x+"]")
#define never (throw std::string("err at ")+(__FILE__)+":"+std::to_string(__LINE__) + " ["+(__func__)+"]")
#endif

namespace ol
{

using vs = ivec<string>;
using ull = unsigned long long;

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
    vs names() const;
};

bool endsWith(string s, string fx);
bool endsWith(string s, string fx, string & cut);
string file2str(const fs::path & file);
string fileHead2str(const fs::path & file, size_t sz);
bool delfile(string file);
unsigned long long filetime(fs::path file);
Msul readdir();
bool replaceAll(string & s, const string & r, const string & to);

struct Warn_t {};
extern Warn_t warn;

} // ol

template<class T> ol::Warn_t & operator<<(ol::Warn_t &, T x);

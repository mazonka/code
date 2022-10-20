#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <map>

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

bool endsWith(string s, string fx);
bool endsWith(string s, string fx, string & cut);

template <class T>
inline size_t x2st(T x) { return static_cast<size_t>( x ); }
string file2str(const string & file);
bool delfile(string file);
int bzip(string file, bool enc);

struct Pushd
{
    fs::path cwd;
    Pushd(string d) { if ( !d.empty() && d != "." ) cwd = fs::current_path(); }
    ~Pushd() { if ( !cwd.empty() ) fs::current_path(cwd); }
    Pushd(string d, fs::path & c) : Pushd(d) { c = cwd; }
};

typedef std::map<std::string, std::pair<unsigned long long, long> > msul;
struct Msul : msul
{
    Msul dirs() const;
    vs names() const;
};
Msul readdir();

} // ol

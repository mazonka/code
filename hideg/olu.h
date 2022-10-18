#pragma once

#include <vector>
#include <string>

using std::string;

template<class T> struct ivec : std::vector<T>
{
    using Base = std::vector<T>;
    int size() const { return (int)Base::size(); }
    void operator+=(const T & b) { Base::push_back(b); }
    ivec operator+(const T & b) { auto r(*this); r.Base::push_back(b); return r; }
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

bool endsWith(string s, string fx);

template <class T>
inline size_t x2st(T x) { return static_cast<size_t>( x ); }
string file2str(const string & file);
bool delfile(string file);
int bzip(string file, bool enc);

} // ol

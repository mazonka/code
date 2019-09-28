#pragma once

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <cctype>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <map>

using std::vector;
using std::string;
using std::cout;

using istr = std::istringstream;
using ostr = std::ostringstream;
using vstr = vector<string>;

using ull = unsigned long long;

inline void cutline(string & line)
{
    while ( line.size() &&
            std::isspace(line[line.size() - 1]) )
        line = line.substr(0, line.size() - 1);
}

template<typename T>
inline string tos(T x) { std::ostringstream o; o << x; return o.str(); }

template<typename T>
inline void merge(T & x, const T & y)
{
    std::sort(x.begin(), x.end());
    T t = y;
    std::sort(t.begin(), t.end());
    T z(x.size() + y.size());
    auto it = std::set_union(x.begin(), x.end(), t.begin(), t.end(), z.begin());
    z.resize(it - z.begin());
    x = z;
}

template<typename T>
inline vector<T> operator+(const vector<T> & x, const vector<T> & y)
{
    vector<T> r;
    r.insert(r.end(), x.begin(), x.end());
    r.insert(r.end(), y.begin(), y.end());
    return r;
}

inline void replaceAll(string & s, const string & r, const string & to)
{
    while (1)
    {
        size_t i = s.find(r);
        if ( i == string::npos ) return;
        s.replace(i, r.size(), to);
    }
}

inline void eatSpaces(string * ps)
{
    string & s = *ps;
    while ( !s.empty() && std::isspace(s[0]) ) s = s.substr(1);
    while ( !s.empty() && std::isspace(s[s.size() - 1]) )
        s = s.substr(0, s.size() - 1);
}

inline string eatSpaces(string s)
{
    string t = s;
    eatSpaces(&t);
    return t;
}

#define never(x) throw "bug at "+tos(__FILE__)+":"+tos(__LINE__) + " ["+tos(x)+"]";

namespace bug // as of today gcc 4.9.2 cygwin does not have to_string
{
template <class T> inline string to_string(T x)
{ std::ostringstream os; os << x; return os.str(); }
inline int stoi(string s) { return std::atoi(s.c_str()); }
} // bug

#ifdef __CYGWIN__
using bug::to_string;
namespace std { inline int stoi(string s) { return bug::stoi(s); } }
#endif

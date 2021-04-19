#include <iostream>
#include <fstream>

struct Coutlog {};
template <class T> inline
Coutlog & operator<<(Coutlog & os, T x)
{
    std::cout << x;
    std::ofstream("cout.log", std::ios::app) << x;
    return os;
}

namespace std { static Coutlog coutlog; }
using std::coutlog;

#define cout coutlog
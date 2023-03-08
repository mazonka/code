#include <iostream>
#include <string>
using std::string;

namespace ol
{
struct Log_t {};
}

ol::Log_t lg;

template<class T>
ol::Log_t & operator<<(ol::Log_t & os, T x);

template<class T>
ol::Log_t & operator<<(ol::Log_t & os, T x)
{
    std::cout << x << '\n';
    return os;
}

template ol::Log_t & operator<< (ol::Log_t & os, int x);
template ol::Log_t & operator<< (ol::Log_t & os, string x);
template ol::Log_t & operator<< <const string & >(ol::Log_t & os, const string & x);
template ol::Log_t & operator<< <const char *>(ol::Log_t & os, const char * x);


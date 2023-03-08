#include <string>
using std::string;

namespace ol
{
struct Log_t {};
}
extern ol::Log_t lg;

template<class T>
ol::Log_t & operator<<(ol::Log_t & os, T x);

int main()
{
    lg << 1;
    lg << string();
    lg << "aaa";
}



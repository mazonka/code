#include <iostream>

template <int X> constexpr void Main() { Main < X - 1 > (); }
template<> void Main<0>() { std::cout << "hi 0" << '\n'; }

int _g_ac;
const char ** _g_av;

int main(int ac, const char * av[])
try
{
    _g_ac = ac;
    _g_av = av;
    Main<800>();
}
catch (...)
{
    std::cout << "Unknown exception\n";
    return 1;
}

template<> void Main<__LINE__>()
{
    std::cout << "Hello, World #1!\n";
}

template<> void Main<__LINE__>()
{
    std::cout << "Hello, World #2!\n";
}

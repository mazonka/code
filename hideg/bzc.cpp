#include <string>
#include <iostream>

using std::string;
using std::cout;

int main_bzc(int ac, const char * av[]);

int main(int ac, const char * av[])
try
{
    return main_bzc(ac,av);
}

catch (int e)
{
    cout << "error (int): " << e << "\n";
    return 1;
}
catch (string e)
{
    cout << "error: " << e << "\n";
    return 1;
}
catch (const char * e)
{
    cout << "error: " << e << "\n";
    return 1;
}
catch (std::exception & e)
{
    cout << "error (std): " << e.what() << "\n";
    return 1;
}
catch (...)
{
    cout << "Unknown exception\n";
    return 1;
}


///#include "hash.cpp"

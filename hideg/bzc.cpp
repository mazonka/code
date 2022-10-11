#include <iostream>

#include "ol.h"

using std::cout;

int main_bzc(ivec<string> args);

int main(int ac, const char * av[])
try
{
    ivec<string> args;
    for_i(ac) args += av[i];
    return main_bzc(args);
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



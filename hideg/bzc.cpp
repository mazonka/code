#include <iostream>

#include "olu.h"

using std::cout;

int main_bzc(string arg0, ivec<string> args);

int main(int ac, const char * av[])
try
{
    ivec<string> args;
    for_i(ac - 1) args += av[i + 1];
    return main_bzc(av[0], args);
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



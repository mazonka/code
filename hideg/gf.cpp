#include <iostream>

#include "ol.h"

using std::cout;

int main_bzc(ivec<string> args);

int main(int ac, const char * av[])
try
{
    ivec<string> args;
    for_i(ac) args += av[i];
    int sz = args.size();
    if ( sz < 1 ) never;
    if ( sz < 2 )
    {
        cout << "gf, ver 1.0.0, Oleg Mazonka 2022";
        cout << "Usage: bzc, hideg";
        return 0;
    }
    auto cmd = args[1];
    args.erase(1);
    if ( cmd == "bzc" ) return main_bzc(args);

    throw "Bad command: " + cmd;
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



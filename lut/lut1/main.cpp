#include <iostream>

#include "alg.h"

using std::cout;

int main(int ac, char * av[])
try
{
    if ( ac < 2 )
    {
        cout << "Usage: build [<name>], run [<name>] [<points>]\n";
        return 0;
    }

    string cmd = av[1];
    string nm = "lut";
    if ( ac > 2 ) nm = av[2];
    string pts;
    if ( ac > 3 ) pts = av[3];

    cout << "cmd = " << cmd << "\nlut = " << nm << "\npts = " << pts << '\n';

    if ( cmd == "build" ) return build(nm);
    else if ( cmd == "run" ) return run(nm, pts);
    else
    {
        cout << "Wrong command [" << cmd << "]\n";
        return 2;
    }

}
catch (int e)
{
    cout << "Internal error: " << e << "\n";
    return 1;
}
catch (const char * e)
{
    cout << "Error: " << e << "\n";
    return 1;
}
catch (string e)
{
    cout << "Error: " << e << "\n";
    return 1;
}
catch (...)
{
    cout << "Error: unknown exception\n";
    return 1;
}


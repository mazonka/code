#include <iostream>
#include <cstdlib>

#ifdef _MSC_VER
#include <direct.h>
#endif

#include "alg.h"

using std::cout;

int main(int ac, char * av[])
try
{
    try
    {
        if (ac < 2)
        {
            cout << "Recursive LUT, Oleg Mazonka, 2018, build 180105\n";
            cout << "Usage: build [n], run [n] [output], cmp [n], draw [n] [level]\n";
            cout << "n = <name>, level default -1\n";
            return 0;
        }

        string cmd = av[1];
        string nm = "lut";
        if (ac > 2) nm = av[2];
        string arg;
        if (ac > 3) arg = av[3];

        cout << "cmd = " << cmd << "\nlut = " << nm;
        if ( !arg.empty() ) cout << "\narg = " << arg << '\n';
        cout << '\n';

        if (0);
        else if (cmd == "build") return build(nm);
        else if (cmd == "run") return run(nm, arg);
        else if (cmd == "cmp") return cmp(nm);
        else if (cmd == "draw") return draw(nm, arg);
        //else if (cmd == "debug") return draw(nm, "-1");
        else if (cmd == "debug")
        {
            Lut t(nm);
            buildLut(t);
            cmpLut(t);
            drawLut(t, arg);
        }
        else
        {
            cout << "Wrong command [" << cmd << "]\n";
            return 2;
        }

    }
    catch (...)
    {
#ifdef _MSC_VER
        char b[1001]; _getcwd(b, 1000); cout << b << '\n';
#endif
        throw;
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


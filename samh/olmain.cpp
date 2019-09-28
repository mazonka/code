#include <iostream>
#include <string>
#include <vector>

#include "os_filesys.h"
#include "olc.h"
#include "gl_except.h"


using namespace std;

using namespace ol;

void olmain0(vector<string> & args);

int main(int ac, char * av[])
try
{
    try
    {
        try
        {
            vector<string> args;
            for (int i = 0; i < ac; i++ ) args.push_back(av[i]);
            olmain0(args);
        }
        catch (...)
        {
            cout << "Current dir: [" << os::FileSys::cwd().str() << "]\n";
            throw;
        }
    }
    catch (int e) { throw "[" + tos(e) + "]"; }
    catch (const char * e) { throw string(e); }
}
catch (string e)
{
    cout << "Error: " << e << "\n";
    return 1;
}
catch (std::exception & e)
{
    cout << "Error (C++ exception) : " << e.what() << '\n';
    return 2;
}
catch (gl::ex & e)
{
    cout << "Error (gl exception) : " << e.str() << '\n';
    return 2;
}
catch (...)
{
    cout << "Error: unknown exception\n";
    return 1;
}


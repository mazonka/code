#include <iostream>
#include <string>
#include <vector>

#include "os_filesys.h"
#include "olc.h"
#include "gl_except.h"


using namespace std;

using namespace ol;

void olmain0(vector<string> & args);

void test();

int main(int ac, char * av[])
try
{
    try
    {
        try
        {
            //test(); return 0;
            vector<string> args;
            for (int i = 0; i < ac; i++ ) args.push_back(av[i]);
            olmain0(args);
        }
        catch (...)
        {
            cout << "\nCurrent dir: [" << os::FileSys::cwd().str() << "]";
            throw;
        }
    }
    catch (int e) { throw "[" + tos(e) + "]"; }
    catch (const char * e) { throw string(e); }
}
catch (string e)
{
    cout << "\nError: " << e << "\n";
    return 1;
}
catch (std::exception & e)
{
    cout << "\nError (C++ exception) : " << e.what() << '\n';
    return 2;
}
catch (gl::ex & e)
{
    cout << "\nError (gl exception) : " << e.str() << '\n';
    return 2;
}
catch (...)
{
    cout << "\nError: unknown exception\n";
    return 1;
}

void test()
{
    os::Dir dir = os::FileSys::readDir(".");
    for ( auto i : dir.files )
    {
        cout << i.second << '\t' << i.first << '\n';
    }
}

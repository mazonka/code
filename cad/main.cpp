#include <iostream>
#include <string>

#include "bad.h"
#include "model.h"


using std::cout;
using std::string;

#ifdef _MSC_VER
#include <direct.h>
#define GETCWD _getcwd
#else
#include <unistd.h>
#define GETCWD getcwd
#endif

const char * getCwd(char * buf, int sz)
{
    const char * r = GETCWD(buf, sz);
    if ( r ) while ( *buf && sz-- > 0 )
        {
            if ( *buf == '\\' ) *buf = '/';
            ++buf;
        }
    return r;
}

string cwd()
{
    const int PATHSZ = 1000;
    char p[PATHSZ];
    const char * r = getCwd(p, PATHSZ);
    if ( !r ) return ("<path_too_long>");

    return (r);
}

void driver(int ac, const char * av[])
{
    string file = av[1];
    cout << "loading " << file << '\n';

    load(file);

    for ( int i = 2; i < ac; i++ )
    {
        string s = av[i];
        cout << "Command: " << s << '\n';

        if (0);

        else if ( s == "save" ) save();
        else if ( s == "loss" ) prn_loss();
        else if ( s == "draw" ) draw();
        else if ( s == "calc" ) calc();

        else throw "Command " + s + " not valid (calc,loss,draw,save)";
    }

    cout << "done\n";
}

int main(int ac, const char * av[])
try
{
    try
    {
        try
        {
            if (ac < 2) model();
            else driver(ac, av);
        }
        catch (...)
        {
            cout << "Current dir: [" << cwd() << "]\n";
            throw;
        }
    }
    catch (int e) { throw "[" + std::to_string(e) + "]"; }
    catch (const char * e) { throw string(e); }
    return 0;
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
catch (...)
{
    cout << "Error: unknown exception\n";
    return 1;
}

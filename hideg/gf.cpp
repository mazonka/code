#include <iostream>
#include <fstream>

#include "olu.h"

using std::cout;

int main_bzc(ivec<string> args);
int main_test(ivec<string> args);

int main(int ac, const char * av[])
try
{
    ivec<string> args;
    for_i(ac) args += av[i];
    int sz = args.size();
    if ( sz < 1 ) never;
    if ( sz < 2 )
    {
        cout << "gf, ver 1.0.2, Oleg Mazonka 2022\n";
        cout << "Usage: bzc, *hideg, test, *ci/co, *gitco/gitci\n";
        return 0;
    }
    auto cmd = args[1];
    args.erase(1);

    if ( cmd == "bzc" ) return main_bzc(args);
    if ( cmd == "test" ) return main_test(args);

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



int main_test(ivec<string> args)
{
    if ( args.empty() ) never;
    if ( args.size() < 2 )
    {
        main_test(args + "bzc");
        return 0;
    }

    if ( args.size() != 2 ) never;

    string mod = args[1];
    args.pop_back();

    cout << "testing " << mod << '\n';
    if ( mod == "bzc" )
    {
        if ( main_bzc(args) ) throw "bad key";

        string fname = "gf.test.tmp";
        string fnameZ = fname + ".bz2";
        string fnameC = fname + ".bzc";

        {
            std::ofstream of(fname, std::ios::binary);
            for ( int i = 0; i < 10000; i++ )
                of.write((const char *)&i, sizeof(int));
        }

        string file_content = ol::file2str(fname);
        cout << "file size " << file_content.size() << '\n';


        ol::delfile(fnameZ);
        if( ol::bzip(fname, true) ) throw "Cannot start bzip2";
        ol::delfile(fname);
        main_bzc(args + "enc" + fnameZ);
        if ( !ol::delfile(fnameZ) ) throw "Cannot delete " + fnameZ;

        main_bzc(args + "dec" + fnameC);
        if ( !ol::delfile(fnameC) ) throw "Cannot delete " + fnameC;
        ol::bzip(fnameZ, false);

        string file_content2 = ol::file2str(fname);

        ol::delfile(fname);

        if ( file_content == file_content2 ) cout << "all good\n";
        else throw "FAILED";

        return 0;
    }

    nevers("unknown test module");
}


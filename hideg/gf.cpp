#include <iostream>
#include <fstream>

#include "olu.h"

using std::cout;

using vs = ivec<string>;

int main_bzc(string arg0, vs args);
int main_test(string arg0, vs args);
int main_hid(string arg0, vs args);
int main_pack(string arg0, vs args, bool pack);

int main(int ac, const char * av[])
try
{
    ivec<string> args;
    for (int i = 1; i < ac; i++) args += av[i];
    int sz = args.size();
    if ( sz < 0 ) never;
    if ( sz < 1 )
    {
        cout << "gf, ver 1.0.6, Oleg Mazonka 2022\n";
        cout << "Usage: bzc, hid, test, pack/unpack, fcl, "
             << "*ci [file]/co [path]/cid, *gitco/gitci\n";
        return 0;
    }
    auto cmd = args[0];
    args.erase(0);

    if (0) {}
    else if ( cmd == "bzc" ) return main_bzc(av[0], args);
    else if ( cmd == "test" ) return main_test(av[0], args);
    else if ( cmd == "hid" ) return main_hid(av[0], args);
    else if ( cmd == "pack" ) return main_pack(av[0], args, true);
    else if ( cmd == "unpack" ) return main_pack(av[0], args, false);


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



int main_test(string arg0, ivec<string> avs)
{
    if ( arg0.empty() ) never;
    if ( avs.size() < 1 )
    {
        main_test(arg0, avs + "bzc");
        return 0;
    }

    if ( avs.size() != 1 ) never;

    string mod = avs[0];
    ///avs.pop_back();

    cout << "testing " << mod << '\n';
    if ( mod == "bzc" )
    {
        if ( main_bzc(arg0, {}) ) throw "bad key";

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
        if ( ol::bzip(fname, true) ) throw "Cannot start bzip2";
        ol::delfile(fname);
        int err = main_bzc(arg0, vs() + "enc" + fnameZ);
        if ( err ) throw "encrypt fail";
        if ( !ol::delfile(fnameZ) ) throw "Cannot delete " + fnameZ;

        if ( main_bzc(arg0, vs() + "dec" + fnameC) ) throw "decrypt fail";
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

int main_pack(string arg0, vs args, bool pack)
{
    if ( arg0.empty() ) never;
    if ( args.size() < 1 )
    {
        cout << "use filename\n";
        return 0;
    }

    if ( args.size() != 1 ) throw "need 1 filename";

    string fname = args[0];
    string fnameZ = fname + ".bz2";

    if ( main_bzc(arg0, {}) ) throw "bad key";

    if (pack)
    {
        if ( ol::bzip(fname, true) ) throw "bzip2 fail";
        if ( main_bzc(arg0, vs() + "enc" + fnameZ) ) throw "encrypt fail";
        if ( !ol::delfile(fnameZ) ) throw "Cannot delete " + fnameZ;
    }
    else // unpack
    {
        if ( !ol::endsWith(fname, ".bzc") ) throw "file is not a pack";
        if ( main_bzc(arg0, vs() + "dec" + fname) ) throw "decrypt fail";
        if ( !ol::delfile(fname) ) throw "Cannot delete " + fname;
        ol::bzip(fname.substr(0, fname.size() - 4) + ".bz2", false);
    }

    return 0;
}


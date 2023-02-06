
#include "gf.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstdlib>

#include "olu.h"
#include "hash.h"
#include "gfu.h"

using std::cout;
namespace fs = std::filesystem;

using vs = ivec<string>;

string g_ver = "gf, v1.4.4, Oleg Mazonka 2022-2023";

inline ol::ull gftime()
{
    ol::ull cftime = ol::filetime(g::gfexe);
    if ( cftime == 0 ) cftime = try_gfexe();
    return cftime;
}

int main(int ac, const char * av[])
try
{
    ivec<string> args;
    for (int i = 1; i < ac; i++) args += av[i];
    int sz = args.size();
    if ( sz < 0 ) never;

    if(ISOPEN) cout<<"[OPEN]\n";
    if ( sz < 1 )
    {
        cout << "Usage  : [options] bzc, g, test, pack/zpaq/cmix/unpack, fcl, setpath\n"
             "       info [file], sync/co/st [@][path|file] [path] (@ - no recursive)\n";
        cout << "Options: -k/-d : keep/discard source file; -s : silent\n";
        cout << "       : -i name/-i ./-iname/-i. : ignore names in co/sync\n";
        return 0;
    }

    while ( args.size() && args[0][0] == '-' )
    {
        auto opt = args[0];
        args.erase(0);
        opt = opt.substr(1);
        if (0) {}
        else if ( opt == "s" ) g::silent = true;
        else if ( opt == "d" ) g::keepfile = 2;
        else if ( opt == "k" ) g::keepfile = 1;
        else if ( opt == "i" )
        {
            if ( args.empty() ) throw "option i requires argument";
            g::ignore += args[0];
            args.erase(0);
        }
        else if ( opt[0] == 'i' ) g::ignore += opt.substr(1);

        else throw "bad option [" + opt + "]";
    }

    auto cmd = args[0];
    args.erase(0);

    g::gfexe = fs::path(av[0]);

    g::keyfilename = "." + g::gfexe.stem().string() + ".key";

    g::root_cwd = fs::current_path();
    if ( cmd == "setpath" ) return main_setpath(args);

    if ( g::sysuid.empty() )
    {
        ol::ull cftime = gftime();
        ///ol::ull cftime = ol::filetime(g::gfexe);
        ///if ( cftime == 0 ) cftime = try_gfexe();
        if ( cftime == 0 ) throw "inaccessible executable, try command 'setpath'";
        g::sysuid = std::to_string(cftime) + g_ver;
    }

    ///g::root_cwd = fs::current_path();

    if (0) {}
    ///else if ( cmd == "setpath" ) return main_setpath(args);
    else if ( cmd == "bzc" ) return main_bzc(args);
    else if ( cmd == "test" ) return main_test(args);
    else if ( cmd == "g" ) return main_hid(args);
    else if ( cmd == "pack" ) return main_pack(args, true);
    else if ( cmd == "unpack" ) return main_pack(args, false);
    else if ( cmd == "fcl" ) return main_fcl(args, (g::keepfile != 2));
    else if ( cmd == "info" ) return main_info(args);
    else if ( cmd == "sync" ) return main_sync(args, 1);
    else if ( cmd == "co" ) return main_sync(args, 2);
    else if ( cmd == "st" ) return main_sync(args, 3);
    else if ( cmd == "zpaq" ) return main_zpaq(args);
    else if ( cmd == "cmix" ) return main_cmix(args);
    ///else if ( cmd == "clean" ) return main_sync(args, 4);

    // shortcut - may remove later
    else if ( cmd == "genkey" ) return main_bzc(vs() + "genkey" + args);


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



int main_test(ivec<string> avs)
{
    ///if ( arg0.empty() ) never;
    if ( avs.size() < 1 )
    {
        main_test(avs + "bzc");
        return 0;
    }

    if ( avs.size() != 1 ) never;

    string mod = avs[0];
    ///avs.pop_back();

    cout << "testing " << mod << '\n';
    if ( mod == "bzc" )
    {
        if ( main_bzc({}) ) throw "bad key";

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
        if ( gfu::bzip(fname, true, false) ) throw "Cannot start bzip2";
        ol::delfile(fname);
        int err = main_bzc(vs() + "enc" + fnameZ);
        if ( err ) throw "encrypt fail";
        if ( !ol::delfile(fnameZ) ) nevers("Cannot delete " + fnameZ);

        if ( main_bzc(vs() + "dec" + fnameC) ) throw "decrypt fail";
        if ( !ol::delfile(fnameC) ) nevers("Cannot delete " + fnameC);
        gfu::bzip(fnameZ, false, false);

        string file_content2 = ol::file2str(fname);

        ol::delfile(fname);

        if ( file_content == file_content2 ) cout << "all good\n";
        else throw "FAILED";

        return 0;
    }

    nevers("unknown test module");
}

int main_pack(vs args, bool pack)
{
    if ( args.size() < 1 )
    {
        cout << "use filename\n";
        return 0;
    }

    if ( args.size() != 1 ) throw "need 1 filename";

    string fname = args[0];

    if ( pack || ol::endsWith(fname, ".g")
            || ol::endsWith(fname, ".bzc") || ol::endsWith(fname, ".zpc") )
    {
        // check key
        if ( main_bzc({}) ) throw "bad key";
    }

    bool isdir = true;
    if ( fs::is_regular_file(fname) ) isdir = false;
    else if ( fs::is_directory(fname) ) {}
    else throw "no file or dir [" + fname + "]";

    bool keep = (g::keepfile == 1);

    if (pack)
    {
        if (isdir)
        {
            if ( main_fcl(vs() + "make" + fname, keep) ) throw "fcl fail";
            fname += ".fcl";
        }

        string fnameZ = fname + ".bz2";
        if ( gfu::bzip(fname, true, keep) ) throw "bzip2 fail";
        if ( main_bzc(vs() + "enc" + fnameZ) ) throw "encrypt fail";

        if ( g::keepfile != 2 )
            if ( !ol::delfile(fnameZ) )
                nevers("Cannot delete " + fnameZ);
    }
    else // unpack
    {
        if ( isdir ) throw "[" + fname + "] is dir";

        static int reent = 0;

        string fncut;

        if (0) {}
        else if ( ol::endsWith(fname, ".bzc", fncut) )
        {
            if ( main_bzc(vs() + "dec" + fname) ) throw "decrypt fail";

            // keepfile 0,1,2: 0-del-exist, 1-keep, 2-del-noexist
            if (g::keepfile == 0)
                if ( !ol::delfile(fname) )
                    nevers("Cannot delete " + fname);

            gfu::bzip( fncut + ".bz2", false, false); // no keep
            fname = fncut;
            if ( ol::endsWith(fname, ".fcl") && !keep ) g::keepfile = 2; // force deletion in fcl
        }
        else if ( ol::endsWith(fname, ".fcl") )
        {
            if ( main_fcl(vs() + "extr" + fname, (g::keepfile != 2)) ) throw "fcl fail";

            // deletion must be taken care inside main_fcl
            ///if (g::keepfile == 2)
            ///if ( !ol::delfile(fname) )
            ///nevers("Cannot delete " + fname);

            return 0; // no descent after fcl
        }
        else if ( ol::endsWith(fname, ".bz2", fncut) )
        {
            gfu::bzip( fname, false, keep);
            fname = fncut;
            if ( ol::endsWith(fname, ".fcl") && !keep ) g::keepfile = 2; // force deletion in fcl
        }
        else if ( ol::endsWith(fname, ".g", fncut) )
        {
            main_hid(vs() + fname);

            if (g::keepfile != 2)
                if ( !ol::delfile(fname) )
                    nevers("Cannot delete " + fname);

            fname = fncut;
        }
        else if ( ol::endsWith(fname, ".zpc", fncut) )
        {
            gfu::zpaq_unpack(fname, true);
            if ( g::keepfile == 2 && !ol::delfile(fname) ) nevers("Cannot delete " + fname);
            fname = fncut;
        }
        else if ( ol::endsWith(fname, ".zpaq", fncut) )
        {
            gfu::zpaq_unpack(fname, false);
            if ( g::keepfile == 2 && !ol::delfile(fname) ) nevers("Cannot delete " + fname);
            return 0; // no descent after zpac
        }
        else if ( ol::endsWith(fname, ".cx", fncut) )
        {
            if ( fs::exists(fncut) ) throw "file [" + fncut + "] exists";
            gfu::cmix(fname, false);
            if ( g::keepfile == 2 && !ol::delfile(fname) ) nevers("Cannot delete " + fname);
            fname = fncut;
        }
        else if ( ol::endsWith(fname, ".gfc", fncut) )
        {
            if ( main_bzc(vs() + "dec" + fname) ) throw "decrypt fail";
            ///if ( !ol::delfile(fname) ) throw "Cannot delete " + fname;
            fname = fncut;
        }
        else
        {
            if ( reent == 0 ) throw "file [" + fname + "] is unpackable";
            return 0; // finish recursion
        }

        ++reent;
        int ret = main_pack(vs() + fname, false);
        --reent;
        return ret;
    }

    return 0;
}

int main_info(vs args)
{
    cout << g_ver << '\n';

    main_bzc({});
    cout << "Binary  = " << g::gfexe.string() << '\n';
    cout << "Keyfile = " << g::keyfile.string() << '\n';

    if(ISOPEN)
    {
        ///cout << "Bintm   = " << ol::filetime(g::gfexe) << '\n';
        cout << "Bintm   = " << gftime() << '\n';
        ///cout << "sysuid  = " << g::sysuid << '\n';
        cout << "hKey    = " << g::hkey << '\n';
    }

    if ( args.empty() ) return 0;

    string file = args[0];

    auto lwt = ol::filetime(file);
    if ( lwt == 0 )
    {
        cout << "Cannot access file " << file << "\n";
        return 1;
    }
    cout << "LWT = " << lwt << "\n";

    if ( !fs::is_regular_file(file) ) return 0;


    string hash = gfu::fileHash(file);
    cout << "hash = " << hash << "\n";

    return 0;
}


int main_zpaq(ivec<string> args)
{
    if ( args.size() != 1 ) throw "Expect filename";
    auto dof = args[0];

    // check key
    if ( main_bzc({}) ) throw "bad key";

    // zpaq add file.zpc file -m5 -key XXX

    if ( g::hkey.empty() ) never;
    string pw = gfu::dkey(2);

    int k = gfu::zpaq(dof, true, pw);
    if ( k == 0 && g::keepfile == 2 ) fs::remove_all(dof);
    return k;
}

int main_cmix(ivec<string> args)
{
    if ( args.size() != 1 ) throw "Expect filename";
    auto file = args[0];

    auto fcx = file + ".cx";
    if ( fs::exists(fcx) ) throw "file [" + fcx + "] exists";
    int k = gfu::cmix(file, true);
    if ( k == 0 && g::keepfile == 2 ) fs::remove(file);
    return k;
}

ol::ull try_gfexe()
{
    string stmpgf = ol::file2str("/tmp/gf.path");
    if ( stmpgf.empty() )
    {
        auto ptmp = std::getenv("TMP");
        if( !ptmp ) return 0;
        fs::path tmp = ptmp;
        stmpgf = ol::file2str(tmp / "gf.path");
    }

    if ( stmpgf.empty() ) return 0;
    string tmpgf;
    {
        std::istringstream is(stmpgf);
        is >> tmpgf;
    }

    auto r = ol::filetime(tmpgf);

    if ( r > 0 ) g::gfexe += "(" + tmpgf + ")";

    return r;
}

int main_setpath(ivec<string> args)
{
    auto exe = g::gfexe.string();
    if ( !fs::exists(g::gfexe) )
    {
        if ( args.empty() )
            throw "inaccessible binary and no path argument"
                  "\nuse 'which' to find path and pass it to setpath as argument"
                  "\ne.g. gf setpath /home/user/bin/gf";
        exe = args[0];
    }

    {
        string sp = "/tmp/gf.path";
        std::ofstream of(sp);
        of << exe << '\n';
        if ( !!of )
        {
            std::cout << "[" << exe << "] saved in " << sp << '\n';
            return 0;
        }
    }

    {
        auto ptmp = std::getenv("TMP");
        if( ptmp )
        {
            fs::path tmp = ptmp;
            tmp = (tmp / "gf.path");

            std::ofstream of(tmp);
            of << exe << '\n';
            if ( !!of )
            {
                std::cout << "[" << exe << "] saved in " << tmp.string() << '\n';
                return 0;
            }
        }
    }

    throw "cannot save path, nether /tmp nor TMP(env) exist; create either";
}

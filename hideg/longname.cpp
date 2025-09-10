#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <memory>

#include "olu.h"
#include "gfu.h"
#include "hash.h"
#include "jadd.h"
#include "chron.h"

using std::cout;

void goDirR(int dpth, fs::path dir);

namespace longname
{

string cmd;
int size;

struct NameStruct
{
    fs::path basename, ext, file;
};

struct DirNames
{
    std::map<string, NameStruct> fnames, conflict;
    int size() { return (int)fnames.size(); }
    void execute() const;
    void add(string fullname, NameStruct ns)
    {
        if ( fullname.empty() ) return;

        if ( fnames.find(fullname) == fnames.end() )
            fnames[fullname] = ns;
        else
            conflict[fullname] = ns;
    }
};

} // long

namespace ln = longname;

int main_long(ivec<string> args)
{
    if ( args.empty() )
    {
        cout << "longname find long names and cut\n";
        cout << "usage: longname {list|cut} num\n";
        cout << "other commands:\n";
        cout << "  cut = cut_back_to_size\n";
        cout << "  cut_back_to_size  - cut back to num size\n";
        cout << "  cut_back_tail     - cut back num of chars\n";
        cout << "  cut_front_to_size - cut front to num size\n";
        cout << "  cut_front_head    - cut front num of chars\n";
        return 0;
    }

    if ( args.size() != 2 ) throw "Bad arguments";

    ln::cmd = args[0];
    ln::size = std::stoi(args[1]);

    if ( ln::size < 1 ) throw "size must be > 0";

    goDirR(1, ".");

    return 0;
}


static long readDirR_cntr = 0;
inline void print_readDirR_cntr()
{
    static auto last = chron::now();
    auto now = chron::now();

    // 100ms cout print
    if ( double(now - last) < 100 ) return;

    last = now;
    cout << readDirR_cntr << '\r';
}

static void proc_detail(ivec<fs::path> files, fs::path cwd)
{
    cout << "CWD: " << cwd.string() << "\n\n";
    for ( auto file : files )
    {
        string sfile = file.string();
        cout << "file      " << file.string() << "\n";
        cout << "filename  " << file.filename().string() << "\n";
        cout << "stem      " << file.stem().string() << "\n";
        cout << "extension " << file.extension().string() << "\n";
        cout << "\n";
    }
}

static void proc_list(ivec<fs::path> files, fs::path cwd)
{
    bool fcwd = true;
    for ( auto file : files )
    {
        string sfile = file.string();
        if ( sfile.size() < ln::size ) continue;
        if ( fcwd ) cout << "[" << cwd.string() << "]\n";
        cout << file.string() << "\n";
        fcwd = false;
    }
}

static ln::DirNames apply_cmd(ivec<fs::path> files)
{
    ln::DirNames data;
    for ( auto file : files )
    {
        ln::NameStruct ns;
        ns.basename = file.stem();
        ns.ext = file.extension();
        ns.file = file;

        string sfile = file.string();
        string sbase = ns.basename.string();
        string sext = ns.ext.string();
        string nbase = sbase;

        int fsz = (int)sfile.size();
        if ( fsz <= ln::size )
        {
            data.add(sfile, ns);
            continue;
        }


        if (0) {}

        else if ( ln::cmd == "cut" || ln::cmd == "cut_back_to_size" )
        {
            // tosize+ext=size => tosize = size - ext
            int tosize = ln::size - (int)sext.size();
            if ( tosize > 0 )
                nbase = sbase.substr(0, tosize);
        }
        else if ( ln::cmd == "cut_front_to_size" )
        {
            int pos = fsz - ln::size;
            if ( pos >= 0 )
                nbase = sbase.substr(pos);
        }
        else if ( ln::cmd == "cut_back_tail" )
        {
            int tosize = (int)sbase.size() - ln::size;
            if ( tosize > 0 )
                nbase = sbase.substr(0, tosize);
            else continue;
        }
        else if ( ln::cmd == "cut_front_head" )
        {
            int pos = ln::size;
            if ( pos >= (int)nbase.size() ) continue;
            nbase = sbase.substr(pos);
        }
        else nevers("bad command [" + ln::cmd + "]");

        data.add(nbase + sext, ns);
    }

    return data;
}

static void process(ivec<fs::path> files, fs::path cwd)
{
    if (0) {}
    else if ( ln::cmd == "detail" ) return proc_detail(files, cwd);
    else if ( ln::cmd == "list" ) return proc_list(files, cwd);

    auto data = apply_cmd(files);
    if ( data.size() != files.size() )
    {
        proc_list(files, cwd);
        string e = "cannot execute operation in [" + cwd.string() + "]";
        cout << e << '\n';

        cout << "Data : " << data.size() << '\n';
        for ( auto x : data.fnames )
        {
            cout << "[" << x.first << "] <- [" << x.second.file << "]\n";
        }
        cout << "conflict : " << data.conflict.size() << '\n';
        for ( auto x : data.conflict )
        {
            cout << "[" << x.first << "] <- [" << x.second.file << "]\n";
        }
        cout << '\n';
        throw e;
    }
    data.execute();
}

static void goDirR(int dpth, fs::path dir)
{
    fs::path pcwd;
    try
    {
        ol::Pushd pushd(dir);
        pcwd = pushd.cwd;
        if (!pushd) nevers("enter dir [" + dir.string() + "] failed");
        auto ents = ol::readdir();

        ivec<fs::path> files;
        for ( auto f : ents.files() )
        {
            ++readDirR_cntr;
            print_readDirR_cntr();
            fs::path pth = f.first;
            //auto size = f.second.second;
            files += pth;
        }

        if (!files.empty()) process(files, pcwd / dir);

        if (g::loaddepth == 0 || g::loaddepth > dpth)
        {
            for (auto d : ents.dirs().names())
            {
                goDirR(dpth + 1, d);
            }
        }
    }
    catch (...)
    {
        static bool printed = false;
        if (printed) throw;
        printed = true;
        cout << "Error in ["
             + pcwd.string() + "] [" + dir.string() + "]\n";
        throw;
    }
}

void ln::DirNames::execute() const
{
    for ( auto f : fnames )
    {
        string newname = f.first;
        auto curname = f.second.file;
        string scurname = curname.string();
        if ( newname == scurname ) continue;
        fs::rename(curname, newname);
        cout << "[" << scurname << "] -> [" << newname << "]\n";
    }
}

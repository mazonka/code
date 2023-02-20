



#include <fstream>
#include <iostream>
#include <set>
#include <vector>

///#include "plib.h"


// PLIB start ------------------------------------------------------
#include <map>
#include <string>
#include <filesystem>
#include <system_error>

#include "olu.h"

using std::string;
namespace fs = std::filesystem;

using llong = long long;

namespace dir
{
inline string getd() { return fs::current_path().string(); }
inline bool setd(string s)
{
    std::error_code e;
    fs::current_path(s, e);
    return !e.default_error_condition();
}
inline void make(string s) { fs::create_directories(s); }

} // dir

// PLIB END ------------------------------------------------


#include "olu.h"

using vs = ivec<string>;


using namespace std;

int make(string);
int extr(string, bool list = false);

namespace opt
{
bool quiet = false;
bool files = false;
string command;
string file;
vector<string> excl;
string dir;
bool parse(int ac, const char ** av)
{
    file = av[ac - 1];
    command = av[1];
    for ( int i = 2; i < ac - 1; i++ )
    {
        string o = av[i];
        if ( o == "-q" ) quiet = true;
        else if ( o == "-f" ) files = true;
        else if ( o == "-D" ) dir = av[++i];
        else if ( o == "-E" ) excl.push_back(av[++i]);
        else if ( o.substr(0, 2) == "-D" ) dir = o.substr(2);
        else if ( o.substr(0, 2) == "-E" )
            excl.push_back(o.substr(2));
        else return false;

        if ( i == ac - 1 ) return false;
    }

    if (command == "make" && dir.empty()) dir = file;

    if ( file.size() > 4
            && ( ol::endsWith(file, ".fcl" )
                 || ol::endsWith(file, ".FCL" ) ) ) {}
    else
        file += ".fcl";

    if ( file[0] == '-' ) return false;

    return true;
}
} // namespace opt

void cfn_read(std::istream & is, string & dest)
{
    const int BSZ = 10000;

    char buf[BSZ + 1];

    is.read(buf, BSZ);

    auto sz = is.gcount();
    string x(buf, sz);

    dest.swap(x);
}

int main_fclc(int ac, const char ** av, bool keep);
int main_fcl(vs args, bool keep)
{
    int sz = args.size();
    const char ** av = new const char* [sz + 1];
    av[0] = nullptr; ///arg0.data();
    for_i(sz) av[i + 1] = args[i].data();
    return main_fclc(sz + 1, av, keep);
}

int main_fclc(int ac, const char ** av, bool keep)
{
    if ( ac < 3 ) goto end;
    if ( !opt::parse(ac, av) )
    {
        cout << "Error: unknown option:";
        for ( int i = 0; i < ac; i++ ) cout << " " << av[i];
        cout << "\n";
        return 1;
    }

    if ( opt::command == "make" )
    {
        int k = make(opt::file);
        if ( k == 0 && !keep ) fs::remove_all(opt::dir);
        return k;
    }
    if ( opt::command == "extr" )
    {
        int k = extr(opt::file);
        if ( k == 0 && !keep ) fs::remove_all(opt::file);
        return k;
    }
    if ( opt::command == "list" ) return extr(opt::file, true);


end:
    cout << "use: make [opts1][opts2] <name> to create archive" << endl;
    cout << "use: extr [opts1] <name> to unpack archive" << endl;
    cout << "use: list <name> to list archive" << endl;
    cout << "opts1: -q (quiet mode) -f (print file mode)\n";
    cout << "opts2: -Ddirectory -Eexclude (-E multiple)\n";
    return 0;

}


ofstream * ofile;
string of_name;

void dir_down(string s);

int make(string s)
{

    // proverim na nalichie
    auto a = ol::readdir();
    if ( a.find(s) != a.end() )
    {
        cout << "file " << s.c_str() << " already exists" << endl;
        return 2;
    }

    if ( opt::dir != "" )
        if ( a.find(opt::dir) == a.end() )
        {
            cout << "no directory " << opt::dir << " in here ("
                 << dir::getd() << ")" << endl;
            return 21;
        }

    ofstream of(s.c_str(), ios::binary);
    if ( !of )
    {
        cout << "cannot open " << s.c_str() << endl;
        return 3;
    }

    ofile = &of;
    of_name = s;

    dir_down(".");

    return 0;
}

vector<string> drs;
void prn_drs(bool endln = true)
{
    for ( vector<string>::iterator i = drs.begin(); i != drs.end(); ++i )
    {
        if ( i != drs.begin() ) cout << "/";
        cout << *i;
    }
    if (endln) cout << endl;
}

void dir_down(string s)
{

    drs.push_back(s);
    if ( !opt::quiet ) if ( s != "." || opt::dir == "" ) prn_drs();

    ol::Msul a;

    string cwd = dir::getd();

    if ( s == "." )
    {
        a = ol::readdir();
        a.erase(of_name);
        if ( opt::dir != "" )
        {
            ol::Msul::iterator d = a.find(opt::dir);
            if ( d == a.end() )
            {
                cerr << "no directory " << opt::dir << " in here" << endl;
                return;
            }
            a.erase(a.begin(), d);
            a.erase(++d, a.end());
        }
    }
    else
    {
        if ( !dir::setd(s) )
        {
            cerr << "cannot enter directory " << s.c_str() << endl;
            return;
        }
        a = ol::readdir();
    }

    //a.erase(".");
    //a.erase("..");
    for ( vector<string>::iterator ei = opt::excl.begin();
            ei != opt::excl.end(); ++ei )
        a.erase(*ei);

    set<string> subdirs;
    set<string> files;

    {
        ol::Msul::iterator i;
        for ( i = a.begin(); i != a.end(); i++ )

            if ( i->second.second == -1L )
                subdirs.insert(i->first);
            else
                files.insert(i->first);

        (*ofile) << files.size() << '\n';
    }

    {
        set<string>::iterator i;
        for ( i = files.begin(); i != files.end(); i++ )
        {

            ifstream in(i->c_str(), ios::binary);
            if ( !in )
            {
                cerr << "cannot open file " << i->c_str() << endl;
                continue;
            }

            if ( opt::files )
            {
                prn_drs(false);
                if ( drs.size() ) cout << "/";
                cout << (*i) << '\n';
            }

            (*ofile) << i->c_str() << '\n';
            auto size_or = a[*i].second;
            (*ofile) << size_or << '\n';

            size_t size = 0;
            string cs;
            while (1)
            {
                cs.clear();
                cfn_read(in, cs);
                if ( !cs.empty() )
                {
                    (*ofile) << cs;
                    size += cs.size();
                }
                if ( !in ) break;
            }
            if ( size != size_or )
            {
                cerr << "file " << i->c_str() << " corrupted" << endl;
                cerr << "size claimed: " << size_or << " , counted: " << size << endl;
                throw "fcl: file [" + (*i) + "] size mismatch";
            }
            (*ofile) << '\n';

        }

        (*ofile) << subdirs.size() << '\n';

        for ( i = subdirs.begin(); i != subdirs.end(); i++ )
        {

            (*ofile) << i->c_str() << '\n';
            dir_down(*i);
        }
    } //

    dir::setd(cwd);

    drs.pop_back();
}


ifstream * ifile;
bool of_error = false;

bool dir_rest();
bool dir_list(string directory);

int extr(string s, bool list)
{

    ifstream in(s, ios::binary);
    if ( !in )
    {
        cout << "cannot open " << s.c_str() << endl;
        return 4;
    }

    ifile = &in;

    bool r;
    if ( list )
        r = dir_list("");
    else
        r = dir_rest();

    char c;
    in.get(c);
    if ( r && !!in )
    {
        cerr << "fcl: input file [" << s << "] underread" << endl;
        return 6;
    }

    if ( r && of_error ) return 7;
    return !r;
}

bool read_int(llong & x)
{

    x = 0;
    char c;
    while (1)
    {
        ifile->get(c);
        if ( !*ifile ) return false;
        if ( c == '\n' ) break;
        x *= 10;
        x += ( c - '0' );
    }
    return true;
}

bool read_string(string & s)
{

    s = "";
    char c;
    while (1)
    {
        ifile->get(c);
        if ( !*ifile ) return false;
        if ( c == '\n' ) break;
        s += c;
    }
    return true;
}

bool dir_rest()
{

    char c;
    llong nfiles = 0;
    if ( !read_int(nfiles) ) goto bad;

    int i;
    for ( i = 0; i < nfiles; i++ )
    {

        string name;
        if ( !read_string(name) ) goto bad;

        if ( opt::files )
        {
            prn_drs(false);
            if ( drs.size() ) cout << "/";
            cout << name << "\n";
        }

        bool of_ok = true;
        ofstream of(name.c_str(), ios::binary);
        if ( !of )
        {
            cerr << "cannot open file " << name.c_str()
                 << " to write" << endl;
            //return false;
            of_error = true;
            of_ok = false;
        }

        llong fsize = 0;
        if ( !read_int(fsize) ) goto bad;

        llong j;

        for ( j = 0; j < fsize; j++ )
        {
            ifile->get(c);
            if ( !*ifile ) goto bad;
            if ( of_ok ) of << c;
        }

        ifile->get(c);
        if ( c != '\n' )
        {
            cerr << "archive file corrupted, read=" << int(c) << " in file " << name << ":" << fsize << endl;
            return false;
        }

    }

    {
        llong ndirs = 0;
        if ( !read_int(ndirs) ) goto bad;


        for ( i = 0; i < ndirs; i++ )
        {
            string name;
            if ( !read_string(name) ) goto bad;

            dir::make(name);
            drs.push_back(name);
            if ( !opt::quiet ) prn_drs();

            string cwd = dir::getd();
            dir::setd(name);
            bool r = dir_rest();
            dir::setd(cwd);

            drs.pop_back();
            if ( !r ) return false;

        }
    } //

    return true;

bad:
    cerr << "unexpected end of archive" << endl;
    return false;
}

//----------------------------------------------------------

bool dir_list(string d)
{

    char c;
    llong nfiles = 0;
    if ( !read_int(nfiles) ) goto bad;

    int i;
    for ( i = 0; i < nfiles; i++ )
    {

        string name;
        if ( !read_string(name) ) goto bad;

        llong fsize = 0;
        if ( !read_int(fsize) ) goto bad;

        (*ifile).seekg(fsize, ios::cur);

        ifile->get(c);
        if ( c != '\n' )
        {
            cerr << "archive file corrupted!" << endl;
            return false;
        }
        cout << fsize << '\t' << (d + name) << endl;

    }

    {
        llong ndirs = 0;
        if ( !read_int(ndirs) ) goto bad;


        for ( i = 0; i < ndirs; i++ )
        {
            string name;
            if ( !read_string(name) ) goto bad;
            cout << "." << '\t' << (d + name) << endl;

            bool r = dir_list(d + name + '/');
            if ( !r ) return false;

        }
    } //

    return true;

bad:
    cerr << "unexpected end of archive" << endl;
    return false;
}

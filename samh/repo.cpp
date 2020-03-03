#include <fstream>

#include "olc.h"
#include "index.h"
#include "copyfile.h"
#include "osfun.h"
#include "samf.h"

const char * reponame = "_samrepo";
const char * repoext = ".sam";
os::Path g_repo;

os::Path findrepo()
{
    auto cwd = os::FileSys::cwd();

    for ( int i = cwd.size() - 1; i >= 0; i-- )
    {
        auto p = os::Path(cwd.strP(i)) + reponame;
        if ( p.isdir() ) return p;
    }

    return "";
}

bool ends_with(const string & a, const string & b)
{
    return a.size() >= b.size() && 0 == a.compare(a.size() - b.size(), b.size(), b);
}

bool starts_with(const std::string & a, const std::string & b)
{
    return a.size() >= b.size() && 0 == a.compare(0, b.size(), b);
}

os::Path makeRepoName(const Hfile & hf)
{
    string slog = std::to_string(std::to_string(hf.file.size).size());
    if ( slog.size() < 2 ) slog = '0' + slog;

    string hash = hf.hash.f;
    string sub = hash.substr(0, 2);

    return g_repo + slog + sub + hash;
}


void checkout_file(string fnsam)
{
    if ( !ends_with(fnsam, repoext) ) throw "Cannot checkout non-sam file";

    string fn = fnsam.substr(0, fnsam.size() - string(repoext).size());

    os::Path pfn(fn);
    if ( pfn.isfile() )
    {
        cout << "Skip [" << fn << "]\n";
        return;
    }

    IndexFile sam(fnsam, true);

    if ( sam.size() != 1 )
        throw "Input file corrupted [" + fnsam + "]";

    auto rpath = makeRepoName(*sam.begin());
    auto srpath = rpath.str();

    if ( !rpath.isfile() ) throw "No file in repository [" + fn + "] [" + srpath + "]";

    copyfile(srpath, fn);

	if( !os::Path(fn).isfile() ) throw "Failed to recover file ["+ fn +"]";

    os::Path(fnsam).erase();
}


void checkin_file(string fn)
{
    if ( ends_with(fn, repoext) ) throw "Cannot checkin sam file";
    sam::File sfile {"", fn, os::FileSys::mtime(fn), os::fileSize(fn) };
    Hfile file(sfile, Hfile::MakeHash);

    auto rpath = makeRepoName(file);

    string srpath = rpath.str();

    if ( !rpath.isfile() )
    {
        sam::moveFile2f(fn, srpath);
        if ( !rpath.isfile() ) throw "Cannot move file to repo [" + fn + "]";
    }
    else
    {
        // check the size
        if ( os::fileSize(srpath) != file.file.size )
        {
            cout << "Critical error detected\n";
            cout << "Repo file: " << srpath << '\n';
            cout << "Local file: " << fn << '\n';
            throw "Corrupted file or repository, sizes mismatch";
        }

        os::Path(fn).erase();
    }

    // update sam file, if need
    string samfn = fn + repoext;
    string old = ol::file2str(samfn, true);
    string s = file.str();
    if ( !starts_with(old, s) ) ol::str2file(samfn, s + '\n' + old);
}

void main_repo(ol::vstr & vcmd)
{
    string file;
    if ( vcmd.size() == 1 ) {}
    else if ( vcmd.size() == 2 ) file = vcmd[1];
    else throw "Too many arguments: only 1 expected";

    auto repo = findrepo();

    if ( repo.empty() )
    {
        cout << "Repository [" << reponame << "] is not found\n";
        cout << "Create repository directory in cwd or above\n";
        throw "no repository found";
    }

    auto cwd = os::FileSys::cwd();
    auto cmd = vcmd[0];

    cout << "command: " << cmd << "\n";
    cout << "repository: [" << repo.str() << "]\n";
    if ( file.empty() )
        cout << "files * in: [" + cwd.str() << "]\n";
    else
        cout << "file: [" + file << "]\n";

    g_repo = repo;

    if (0) {}

    else if ( cmd == "checkin" )
    {
        if ( !file.empty() ) return checkin_file(file);

        extern bool inclDot;
        sam::mfu files = sam::getListOfFiles(cwd, inclDot, ol::vstr{reponame});

        for ( auto i : files )
        {
			auto f = i.first;
            if ( ends_with(f.fname, repoext) ) continue;
            if ( ends_with(f.dname, reponame) ) continue;
            checkin_file(f.name());
        }
    }

    else if ( cmd == "checkout" )
    {
        if ( !file.empty() ) return checkout_file(file);

        extern bool inclDot;
        sam::mfu files = sam::getListOfFiles(cwd, inclDot, ol::vstr{reponame});

        for ( auto i : files )
        {
            if ( !ends_with(i.first.fname, repoext) ) continue;
            if ( ends_with(i.first.dname, reponame) ) continue;
            checkout_file(i.first.name());
        }
    }
}

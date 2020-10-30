#include <fstream>

#include "olc.h"
#include "index.h"
#include "copyfile.h"
#include "osfun.h"
#include "samf.h"

const char * reponame = "_samr";
const char * dotsam = ".sam";
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

string cut_end(const string & a, const string & b)
{
    if ( a.size() <= b.size() ) return "";
    auto c = a.substr(0, a.size() - b.size());
    if ( c + b == a ) return c;
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

os::Path makeRepoNameX(const Hfile & hf)
{
    string slog = std::to_string(std::to_string(hf.file.size).size());
    if ( slog.size() < 2 ) slog = '0' + slog;

    string hash = hf.hash.f;
    string sub = hash.substr(0, 2);

    return g_repo + slog + sub + hash;
}

os::Path makeRepoName(string hash)
{
    string sub1 = hash.substr(0, 2);
    string sub2 = hash.substr(2, 2);

    return g_repo + sub1 + sub2 + hash;
}

bool hashok(const string & h)
{
    int sz = 64;
    if ( (int)h.size() != sz ) return false;
    return ol::isHex(h);
}

void checkout_file(string fnsam, bool erase)
{
    if ( !ends_with(fnsam, dotsam) ) throw "Cannot checkout non-sam file";

    string fn = fnsam.substr(0, fnsam.size() - string(dotsam).size());

    os::Path pfn(fn);
    if ( pfn.isfile() )
    {
        cout << "Skip [" << fn << "]\n";
        return;
    }

    os::Path rpath;
    {
        std::ifstream in(fnsam);
        string h; in >> h;
        if ( !in ) throw "Bad file access [" + fnsam + "]";
        if ( !hashok(h) ) throw "File corrupted [" + fnsam + "]";
        rpath = makeRepoName(h);
    }

    auto srpath = rpath.str();

    if ( !rpath.isfile() ) throw "No file in repository [" + fn + "] [" + srpath + "]";

    copyfile(srpath, fn);

    if ( !os::Path(fn).isfile() ) throw "Failed to recover file [" + fn + "]";

    if ( erase ) os::Path(fnsam).erase();
}

void checkoutr_file(string fnsam) { checkout_file(fnsam, true); }
void checkoutk_file(string fnsam) { checkout_file(fnsam, false); }

os::Path get_sam_rpath(string fnsam)
{
    std::ifstream in(fnsam);
    string h; in >> h;
    if ( !in ) throw "Bad file access [" + fnsam + "]";
    if ( !hashok(h) ) throw "File corrupted [" + fnsam + "]";
    return makeRepoName(h);
}

void checkout_move(string fnsam)
{
    if ( !ends_with(fnsam, dotsam) ) throw "Cannot checkout non-sam file";

    string fn = fnsam.substr(0, fnsam.size() - string(dotsam).size());

    os::Path pfn(fn);
    if ( pfn.isfile() )
    {
        cout << "Skip [" << fn << "]\n";
        return;
    }

    os::Path rpath = get_sam_rpath(fnsam);

    auto srpath = rpath.str();

    if ( !rpath.isfile() )
    {
        cout << "No file in repo [" + fn + "]\n";
        std::ofstream("sam.comove.log", std::ios::app) << fn << '\n';
        return;
    }


    os::FileSys::move(srpath, fn);

    if ( !os::Path(fn).isfile() ) throw "Failed to recover file [" + fn + "]";
}

void checkout_mv_delsam(string fnsam)
{
    checkout_move(fnsam);
    os::Path(fnsam).erase();
}

void checkout_mv_keepsam(string fnsam)
{
    checkout_move(fnsam);
}

bool rmfile(string t)
{
    for ( int i = 0; i < 100; i++ )
    {
        if ( os::FileSys::erase(t) ) return true;
        os::sleep(10);
    }
    if ( os::FileSys::erase(t) ) return true;
    return false;
}

bool rnfile(string o, string n)
{
    dirForFile(n);
    for ( int i = 0; i < 100; i++ )
    {
        if ( os::rename(o, n) ) return true;
        os::sleep(10);
    }
    if ( os::rename(o, n) ) return true;
    return false;
}


void checkin_file_move_to_repo(string hash, string fn, gl::sll size)
{
    auto rpath = makeRepoName(hash);

    string srpath = rpath.str();

    if ( !rpath.isfile() )
    {
        rnfile(fn, srpath);
        if ( !rpath.isfile() ) throw "Cannot move file to repo [" + fn + "]";
    }
    else
    {
        // check the size
        if ( os::fileSize(srpath) != size )
        {
            cout << "Critical error detected\n";
            cout << "Repo file: " << srpath << '\n';
            cout << "Local file: " << fn << '\n';
            throw "Corrupted file or repository, sizes mismatch";
        }

        if ( !rmfile(fn) ) cout << "Not erased [" << fn << "]\n";
    }
}

void checkin_file(string fn)
{
    if ( ends_with(fn, dotsam) ) throw "Cannot checkin sam file";

    sam::File sfile {"", fn, os::FileSys::mtime(fn), os::fileSize(fn) };
    Hfile file(sfile, Hfile::MakeHashF);

    checkin_file_move_to_repo(file.hash.f, fn, file.file.size);

    // update sam file, if need
    string samfn = fn + dotsam;
    string old = ol::file2str(samfn, true);
    string s = file.hash.f;
    if ( !starts_with(old, s) )
    {
        string c = s + '\n' + old;
        if ( !ol::str2file(samfn, c) )
        {
            string emer = fn + filetmpext + dotsam;
            ol::str2file(emer, c);
            throw "Access denied to [" + samfn + "]. Tring to save to [" + emer
            + "]. Please fix manually. File in repository [" + s + "]";
        }
    }
}

void checkinr_file(string fn) { checkin_file(fn); }

void checkink_file(string fn)
{
    if ( ends_with(fn, dotsam) ) throw "Cannot checkin sam file";

    string fnsam = fn + dotsam;

    string h;
    {
        std::ifstream in(fnsam);
        in >> h;
        if ( !in ) return checkin_file(fn);
        if ( !hashok(h) ) throw "File corrupted [" + fnsam + "]";
    }

    checkin_file_move_to_repo(h, fn, os::fileSize(fn));
}

void checkin_mv(string fnsam)
{
    // 1 remove .sam
    string fn = cut_end(fnsam, dotsam);

    if ( fn.empty() ) never("not sam file");

    // 2 if no file, nothing
    os::Path pfn(fn);
    if ( !pfn.isfile() ) return; // skip - must be in

    // 3 find hash
    os::Path rpath = get_sam_rpath(fnsam);

    // 4 prepare dir
    // 5 move file
    string srpath = rpath.str();

    if ( rpath.isfile() )
    {
        cout << "Warning: file in repo; not erased [" << fn << "]\n";
        return;
    }

    rnfile(fn, srpath);
    if ( !rpath.isfile() ) throw "Cannot move file to repo [" + fn + "]";
}



void sub_repo(ol::vstr & vcmd);

void main_repo(ol::vstr & vcmd)
{
    string file;
    if ( vcmd.size() == 1 ) {}
    else if ( vcmd.size() == 2 ) file = vcmd[1];

    auto repo = findrepo();

    if ( repo.empty() )
    {
        cout << "Repository [" << reponame << "] is not found\n";
        cout << "Create repository directory in cwd or above\n";
        throw "no repository found";
    }
    g_repo = repo;

    auto cwd = os::FileSys::cwd();
    auto cmd = vcmd[0];

    if ( cmd == "repo" ) return sub_repo(vcmd);

    if ( vcmd.size() > 2 ) throw "Too many arguments: only 1 expected";

    cout << "command: " << cmd << "\n";
    cout << "repository: [" << repo.str() << "]\n";
    if ( file.empty() )
        cout << "files * in: [" + cwd.str() << "]\n";
    else
        cout << "file: [" + file << "]\n";

    void (*chkf[7])(string) =
    {
        checkinr_file, checkoutr_file,
        checkout_mv_delsam, checkout_mv_keepsam, checkin_mv,
        checkink_file, checkoutk_file
    };

    int idx = -1;

    if (0) {}
    else if ( cmd == "checkin" || cmd == "cir" ) idx = 0;
    else if ( cmd == "checkout"  || cmd == "cor" ) idx = 1;
    else if ( cmd == "comove" ) idx = 2;
    else if ( cmd == "comv" ) idx = 3;
    else if ( cmd == "cimv" ) idx = 4;
    else if ( cmd == "cik" ) idx = 5;
    else if ( cmd == "cok" ) idx = 6;
    else throw "Unknown command 308";

    auto dir = cwd;
    if ( !file.empty() )
    {
        auto f = os::Path(file);
        if ( f.isfile() ) return chkf[idx](file);
        else if ( f.isdir() ) dir = file;
        else throw "Bad argument [" + file + "]";
    }

    extern bool inclDot;
    sam::mfu filesall = sam::getListOfFiles(dir, inclDot, ol::vstr {reponame});
    sam::mfu files;

    for ( auto i : filesall )
    {
        auto f = i.first;

        if ( ends_with(f.fname, filetmpext) )
            throw "Temporary file found [" + f.name() + "]";

        if ( ends_with(f.dname, reponame) ) continue;

        bool isdotsam = ends_with(f.fname, dotsam);
        bool ischksam = ( idx == 0 || idx == 5 );

        if ( isdotsam == ischksam ) continue;

        files.insert(i);
    }

    Timer timer;
    int cntr = 0;
    auto sz = files.size();

    for ( auto i : files )
    {

        ++cntr;
        if ( timer.get() > 500 )
        {
            timer.init();
            cout << cntr << "/" << sz << '\r';
        }

        auto f = i.first;
        ///if ( ends_with(f.fname, dotsam) == !idx ) continue;
        ///if ( ends_with(f.dname, reponame) ) continue;
        chkf[idx](f.name());

        if ( os::kbhit() == 27 )
        {
            cout << "Interrupted at " << cntr << "/" << sz << "\n";
            return;
        }
    }
    cout << sz << "/" << sz << '\n';
}

sam::mfu getAllFilesFromDir(ol::vstr & vcmd)
{
    string dir = ".";
    if ( vcmd.size() > 2 )
    {
        dir = vcmd[2];
        if ( !os::Path(vcmd[2]).isdir() ) throw "Not directory [" + vcmd[2] + "]";
    }

    extern bool inclDot; // use true
    sam::mfu files = sam::getListOfFiles(dir, true, ol::vstr {reponame});
    return files;
}

void sub_repo_check(ol::vstr & vcmd, bool flost)
{
    std::map<string, int> mr;
    if (flost) // read all hashes
    {
        sam::mfu hashes = sam::getListOfFiles(reponame, true, ol::vstr());
        if ( hashes.empty() )
            throw string() + "Run in top dir: where "
            + reponame + " is.";
        for ( auto x : hashes ) mr[x.first.fname] = 0;
    }

    sam::mfu files = getAllFilesFromDir(vcmd);

    Timer timer;
    int cntr = 0;
    auto sz = files.size();

    for ( auto i : files )
    {
        auto f = i.first;

        if ( ends_with(f.fname, filetmpext) )
            throw "Temporary file found [" + f.name() + "]";

        if ( !ends_with(f.fname, dotsam) ) continue;
        if ( ends_with(f.dname, reponame) ) continue;

        if ( !flost )
        {
            os::Path rfile = get_sam_rpath(f.name());
            if ( !rfile.isfile() ) cout << f.name() << "\n";
        }
        else
        {
            string hash = ol::eatSpaces(ol::file2str(f.name()));
            ++mr[hash];
        }

        ++cntr;
        if ( timer.get() > 500 )
        {
            timer.init();
            cout << cntr << "/" << sz << '\r';
        }

        if ( os::kbhit() == 27 )
        {
            cout << "Interrupted at " << cntr << "/" << sz << "\n";
            return;
        }
    }
    cout << sz << "/" << sz << '\n';

    if (!flost) return;

    int nlost = 0;
    for ( auto [k, v] : mr )
        if (!v) { ++nlost;  cout << k << '\n'; }

    if ( nlost)
        cout << "Lost files found: " << nlost << "\n";
    else
        cout << "All repo files referenced\n";
}

void sub_repo(ol::vstr & vcmd)
{
    if ( vcmd.size() < 2 )
    {
        cout << "findnosam - find files that are not sam extension\n";
        cout << "cleanup (NI) - clean repository with respect to directory\n";
        cout << "erase (NI) - erase repository data\n";
        cout << "check - check that all sam have file in repo\n";
        cout << "lost - list not referenced files in repo (in root)\n";
        return;
    }

    string cmd = vcmd[1];

    if (0) {}

    else if ( cmd == "findnosam" )
    {
        sam::mfu files = getAllFilesFromDir(vcmd);

        for ( auto i : files )
        {
            auto f = i.first;

            if ( ends_with(f.fname, filetmpext) )
                throw "Temporary file found [" + f.name() + "]";

            if ( !ends_with(f.fname, dotsam) ) cout << f.name() << '\n';
            if ( ends_with(f.dname, reponame) ) continue;
        }
    }
    else if ( cmd == "check" ) sub_repo_check(vcmd, false);
    else if ( cmd == "lost" ) sub_repo_check(vcmd, true);

    else throw "Bad repo command [" + cmd + "]";
}


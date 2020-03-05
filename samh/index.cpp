#include <fstream>

#include "olc.h"
#include "index.h"
#include "copyfile.h"
#include "osfun.h"
#include "samf.h"

using std::flush;

Hfile::Hfile(sam::File f, Hfile::MakeHash_t mh): file(f), hash()
{
    string qh, fh;
    if ( mh & MakeHashQ ) qh = qhash(f);
    if ( mh & MakeHashF ) fh = fhash(f);
    hash = QfHash {qh, fh};
}

void main_index(ol::vstr & av)
{
    cout << "Command: "; for ( auto i : av ) cout << i << ' '; cout << '\n';

    if ( av.empty() ) av.push_back("help");

    string cmd = av[0];
    if ( !av.empty() ) av.erase(av.begin());

    if ( cmd == "help" )
    {
        std::cout << "index commands: gen, same, fix, valid, split\n";
        std::cout << "gen indexfile [cwd=.]\n";
        return;
    }

    else if ( cmd == "gen" )
    {
        void index_gen(ol::vstr & av);
        index_gen(av);
    }

    else if ( cmd == "same" )
    {
        void index_same(ol::vstr & av);
        index_same(av);
    }

    else if ( cmd == "rmsame" )
    {
        void index_rmsame(ol::vstr & av);
        index_rmsame(av);
    }

    else if ( cmd == "fix" || cmd == "check" )
    {
        void index_fix(ol::vstr & av, bool isfix);
        index_fix(av, cmd == "fix");
    }

    else if ( cmd == "split" )
    {
        void index_split(ol::vstr & av);
        index_split(av);
    }

    else if ( cmd == "valid" )
    {
        void index_valid(ol::vstr & av);
        index_valid(av);
    }

    else if ( cmd == "addindex" )
    {
        void index_addidx(ol::vstr & av);
        index_addidx(av);
    }

    else throw "Bad index command [" + cmd + "]";
}

void index_gen(ol::vstr & av)
{
    if ( av.size() < 1 )
    {
        cout << "Usage : gen index_file [dir]\n";
        cout << "      : gen dir\n";
        throw "index filename or directory expected";
    }

    string indexfn = av[0];
    string cwd = ".";
    if ( av.size() > 1 ) cwd = av[1];
    extern bool inclDot;

    if ( os::isDir(indexfn) )
    {
        if ( av.size() != 1 ) throw "No more args";
        cwd = indexfn;
        indexfn = indexfn + ".sam";
    }

    if ( os::isFile(indexfn) ) throw "File [" + indexfn + "] exists, use fix to update";

    sam::mfu files = sam::getListOfFiles(cwd, inclDot);

    int sz = (int)files.size();
    int cntr = 0;
    string prog;
    cout << '\n';

    cout << "Generating hash table (press Esc to interrupt)\n";
    bool inter = false;

    std::ofstream of(indexfn, std::ios::binary);
    if ( !of ) throw "Cannot open file [" + indexfn + "] for writing";

    for ( auto fi : files )
    {
        cntr++;
        const auto & f =  fi.first;
        auto name = f.name();
        if ( name == indexfn ) continue;

        Hfile hf(f, Hfile::MakeHashQF);

        of << hf.str() << '\n';

        string prg = ol::tos(int(100.0 * cntr / sz));
        if ( prg != prog )
        {
            prog = prg;
            cout << prog << "%\r";
        }

        if ( os::kbhit() == 27 )
        {
            inter = true;
            break;
        }
    }

    if ( inter )
        cout << "\nInterrupted, index file [" << indexfn << "] is not complete\n";
    else
        cout << "\nIndex file [" << indexfn << "] complete\n";
}

/*///
string QfHash::str() const
{
    ol::ostr os;

    os << q << '\n';
    os << f << '\n';

    return os.str();
}
*/

void cwd2slash(string & s, bool to)
{
    if (to)
    {
        if ( s.empty() ) s = "./";
    }
    else
    {
        if ( s == "./" ) s = "";
    }
}

string Hfile::str() const
{
    ol::ostr os;
    auto dname = file.dname;
    cwd2slash(dname, true);

    os << dname << '\n';
    os << file.fname << '\n';
    os << file.mtime << '\n';
    os << file.size << '\n';

    ///os << hash.str();
    os << hash.q << '\n';

    if ( hash.f == hash.q )
        os << "=\n";
    else
        os << hash.f << '\n';

    return os.str();
}

IndexFile::IndexFile(string f, bool top) : filename(f)
{
    std::ifstream in(f);
    if ( !in ) throw "Cannot open " + f;

    while (1)
    {
        string dir, name, mtime, ssize, qh, fh, x;
        std::getline(in, dir);
        cwd2slash(dir, false);
        std::getline(in, name);
        std::getline(in, mtime);
        std::getline(in, ssize);
        std::getline(in, qh);
        std::getline(in, fh); if (fh == "=") fh = qh;
        std::getline(in, x);
        if ( !in ) break;
        sam::File sf {dir, name, (time_t)std::stoull(mtime), std::stoll(ssize)};

        if ( qh.empty() || fh.empty() ) throw "Index file [" + f + "] corrupted";

        (*this)[sf] = QfHash {qh, fh};

        if ( top ) break;
    }
}

void IndexFile::save(string f) const
{
    if ( f.empty() ) throw "Cannot save index file without name";
    std::ofstream of(f, std::ios::binary);
    for ( const auto & i : (*this) )
    {
        // do not save empty hash
        if ( i.second.f.empty() ) continue;
        of << Hfile(i).str() << '\n';
    }
}

using Mumsh = std::map < ol::ull, std::map< string, std::vector<Hfile> > >;
using Mff = std::map< sam::File, std::vector<sam::File> >;
Mumsh getSameFiles(string indexfn, Mff * susp)
{
    Mumsh b;
    Mff b2;
    {
        std::map < string, std::vector<Hfile> > m;
        {
            IndexFile fi(indexfn);

            cout << "Loaded index : " << fi.size() << '\n';

            for ( const auto & i : fi )
            {
                m[i.second.f].push_back(Hfile {i});
                if ( susp )
                {
                    sam::File fil(i.first);
                    fil.dname = "";
                    fil.mtime = 0;
                    b2[fil].push_back(i.first);
                }
            }
        }

        for ( const auto & i : m )
        {
            if ( i.second.size() < 2 ) continue;
            b[i.second[0].file.size][i.first] = i.second;
        }

    }

    if ( susp )
    {
        for ( const auto & i : b2 )
        {
            if ( i.second.size() < 2 ) continue;
            (*susp)[i.first] = i.second;
        }
    }

    return b;
}

void index_same(ol::vstr & av)
{
    if ( av.size() < 1 ) throw "index filename expected";
    string indexfn = av[0];

    Mff mff;
    auto b = getSameFiles(indexfn, &mff);

    if ( b.empty() && mff.empty() )
    {
        cout << "No same files found\n";
        return;
    }

    int nons = 0;
    if ( !mff.empty() )
    {
        cout << "Suspicious files found: \n";
        for ( const auto & i : mff )
        {
            cout << "\nSuspicious size=" << i.first.size << '\n';
            for ( auto j : i.second ) cout << j.name() << '\n';
            nons += (int)i.second.size();
        }
    }

    int nonu = 0;
    for ( const auto & i : b )
    {
        for ( const auto & k : i.second )
        {
            cout << "\nSame h=" << k.first << " size=" << i.first << '\n';
            for ( const auto & j : k.second ) cout << j.file.name() << '\n';
            nonu += (int)k.second.size();
        }
    }

    cout << "\nTotal number of non-unique files: " << nonu;
    cout << "\nTotal number of suspicious files: " << nons << '\n';
}

void index_rmsame(ol::vstr & av)
{
    if ( av.size() < 1 ) throw "index_file [path_prefix]";
    string indexfn = av[0];

    string pref;
    if ( av.size() > 1 ) pref = av[1];

    auto b = getSameFiles(indexfn, nullptr);

    if ( b.empty() )
    {
        cout << "No same files found\n";
        return;
    }

    int nonu = 0;
    if ( pref.empty() )
    {
        for ( const auto & i : b )
        {
            for ( const auto & k : i.second )
            {
                cout << "\nSame h=" << k.first << " size=" << i.first << '\n';
                ///cout << "\nSame files of size: " << i.first << '\n';
                bool first = true;
                for ( const auto & j : k.second )
                {
                    auto f = j.file.name();

                    if ( first )
                    {
                        first = false;
                        cout << "==> " << f << '\n';
                        continue;
                    }

                    bool rmed = os::FileSys::erase(f); ++nonu;

                    if (rmed)
                        cout << "rm: " << f << '\n';
                    else
                        cout << "FAILED to rm: " << f << '\n';
                }
            }
        } // for
    }
    else
    {
        for ( const auto & i : b )
        {
            for ( const auto & k : i.second )
            {
                ///cout << "\nSame files of size: " << i.first << '\n';
                auto psz = pref.size();
                bool allrmed = true;
                for ( int j = (int)(k.second.size() - 1); j >= 0; j-- )
                {
                    auto f = k.second[j].file.name();

                    if ( j == 0 && allrmed ) break;

                    bool pbig = (psz > f.size());
                    if ( pbig || f.substr(0, psz) != pref )
                    {
                        allrmed = false;
                        continue;
                    }

                    bool rmed = os::FileSys::erase(f); ++nonu;

                    if (rmed)
                        cout << "rm: " << f << '\n';
                    else
                        cout << "FAILED to rm: " << f << '\n';
                }
            }
        } // for
    }

    cout << "\nTotal number of removed files: " << nonu << '\n';
}

void processCode(string code, IndexFile & idi, IndexFile & ifh, IndexFile & inf)
{
    string pro = "Code " + code + ": ";
    cout << pro << '\r';
    bool M = (code.find("M") == string::npos);
    bool R = (code.find("R") == string::npos);
    bool A = (code.find("A") == string::npos);
    bool H = (code.find("H") != string::npos);
    ol::replaceAll(code, "M", "");
    ol::replaceAll(code, "R", "");
    ol::replaceAll(code, "A", "");
    ol::replaceAll(code, "H", "");
    if ( !code.empty() ) throw "Bad MRAH-code";

    int resolved = 0;
    Timer timer;
    int cntr = 0;
    for (auto i = inf.cbegin(), nxti = i; i != inf.cend(); i = nxti)
    {
        ++cntr;
        if ( timer.get() > 500 )
        {
            timer.init();
            cout << pro << cntr << "/" << inf.size() << '\r';
        }

        ++nxti;

        const auto & nf = i->first;
        Hfile candidate;
        int found = 0;
        for ( const auto & j : ifh )
        {
            const auto & hf = j.first;
            if ( nf.size != hf.size ) continue;
            if ( M && nf.dname != hf.dname ) continue;
            if ( R && nf.fname != hf.fname ) continue;
            if ( A && nf.mtime != hf.mtime ) continue;
            if ( H )
            {
                auto iter = idi.find(nf);
                if ( iter == idi.end() ) throw "Internal error 237";
                string qh = qhcache(*iter);
                if ( qh != j.second.q ) continue;
            }

            // we found a candidate
            // if this candidate a second one and has the same hash,
            // the ignore it because it is the same file, a duplicate
            if ( found == 1 && candidate.hash == j.second ) continue;

            candidate = Hfile(j);
            if ( ++found > 1 ) break;
        }

        ///if(0) if( found > 1 ) cout<<'.';

        if ( found != 1 ) continue;
        idi[nf] = candidate.hash;

        inf.erase(i);
        ifh.erase(candidate.file);
        resolved++;
    }

    cout << pro << "resolved: " << resolved << '\n';
}

void index_fix(ol::vstr & av, bool isfix)
{
    if ( av.size() < 1 )
    {
        cout << "Usage : fix index_file dir [MRAH-code]\n";
        cout << "      : fix dir\n";
        throw "filename or dir expected";
    }

    string indexfn = av[0];
    string cwd = ".";
    if ( av.size() > 1 ) cwd = av[1];
    extern bool inclDot;

    if ( os::isDir(indexfn) )
    {
        if ( av.size() != 1 ) throw "No more args";
        cwd = indexfn;
        indexfn = indexfn + ".sam";
    }

    // cwd must be a real directory
    if ( !os::isDir(cwd) ) throw "Cannot read dir [" + cwd + "]";

    if ( isfix )
    {
        cout << "MRAH-codes:";
        if ( av.size() < 3 )
            cout << " (none)";
        else
            for ( int i = 2; i < (int)av.size(); i++ ) cout << ' ' << av[i];
        cout << '\n';
    }

    // order of priority to check
    // 0. size - a must
    // 1. access
    // 2. fname
    // 3. dir

    // algorithm: Read files from dir and generate new index (DI) with
    // empty qh anf fh. Read index and put it in File:Hfile map (FH).
    // Go through each in DI and fill matching FH. On the way
    // construct non-matching list, and exclude matched from FH.
    // Next, for each MRAH-code find more matches.
    // Finally, calculate all hashes.
    // Save if fixflag

    sam::mfu files = sam::getListOfFiles(cwd, inclDot);

    IndexFile di;
    int cntr = 0;

    cout << "Resolving index" << flush;
    for ( auto fi : files )
    {
        const auto & f =  fi.first;
        auto name = f.name();
        if ( name == indexfn ) continue;
        di[f] = QfHash();
    }

    IndexFile fh(indexfn), notfound;

    for ( auto i : di )
    {
        auto file = i.first;
        auto found = fh.find(file);
        if ( found == fh.end() )
        {
            notfound[file];
        }
        else
        {
            di[file] = found->second;
            fh.erase(found);
            cntr++;
        }
    }
    if ( notfound.empty() )
        cout << ": all " << cntr << " resolved\n";
    else
        cout << ": " << cntr << " resolved, " << notfound.size() << " remained\n";
    cntr = 0;


    if ( fh.empty() && notfound.empty() )
    {
        cout << "Index [" << indexfn << "] ok\n";
        return;
    }

    for ( int i = 2; i < (int)av.size(); i++ )
    {
        string code = av[i];
        processCode(code, di, fh, notfound);
    }

    if (0)
    {
        for (auto & i : notfound) i.second = QfHash {"?", "?"};
        notfound.save("z_nf");
    }

    if ( !isfix )
    {
        // validate
        cout << "Extra index found: " << fh.size() << '\n';
        //for ( auto i : fh ) cout << i.first.name() << '\n';

        cout << "Out of index: " << notfound.size() << '\n';
        if ( notfound.size() < 10 )
        {
            for ( auto i : notfound )
                cout << i.first.name() << '\n';
        }
        return;
    }

    cout << "Checking " << notfound.size() << " unresolved files (press Esc to interrupt)\n";
    int disz = (int)di.size();
    cntr = 0;
    Timer timer;
    for ( auto & i : di )
    {
        if ( i.second.f.empty() )
        {
            auto sz = i.first.size;
            //cout << "size " << (sz) << ' ';
            const ol::ull mb = 1024ull * 1024;
            if ( sz > mb * 100 )
                cout << "hashing " << (sz / mb) << "M: " << i.first.name() << '\n';
        }
        ///else cout << "." << std::flush;

        (void)qhcache(i);
        (void)fhcache(i);

        if ( os::kbhit() == 27 )
        {
            cout << "\nInterrupted, index file [" << indexfn << "] is not complete\n";
            break;
        }

        ++cntr;
        if ( timer.get() > 100 )
        {
            timer.init();
            cout << cntr << "/" << disz << '\r';
        }
    }

    cout << "Saving index [" << indexfn << "] " << flush;
    di.save(indexfn);
    cout << di.size() << "\n";
}

void index_split(ol::vstr & av)
{
    if ( av.size() != 3 ) throw "target_index src_index dirname";
    string tar_idx = av[0];
    string src_idx = av[1];
    string dirname = av[2];
    string dirold = dirname + ".old";
    string dirnew = dirname + ".new";

    if ( dirname == "." || dirname == ".."
            || dirname.find("/") != string::npos )
        throw "Dirname must be a simple name";

    cout << "Target index : " << tar_idx << '\n';
    cout << "Source index : " << src_idx << '\n';
    cout << "Dir name : " << dirname << '\n';
    cout << "Dir for old : " << dirold << '\n';
    cout << "Dir for new : " << dirnew << '\n';

    IndexFile tar(tar_idx);
    IndexFile src(src_idx);

    std::map<string, int> mtar;
    for ( auto i : tar ) mtar[i.second.f] = 1;

    extern bool inclDot;
    sam::mfu files = sam::getListOfFiles(dirname, inclDot);

    int cnerr = 0, cnold = 0, cnnew = 0;
    for ( const auto & i : files )
    {
        auto iter = src.find(i.first);
        if ( iter == src.end() )
        {
            cnerr++;
            cout << "Warning: file is not indexed [" << i.first.name() << "]\n";
            continue;
        }

        string h = iter->second.f;
        auto found = mtar.find(h);

        string dir = dirold; cnold++;
        if ( found == mtar.end() ) { dir = dirnew; cnold--; cnnew++; }
        sam::moveFile2d(i.first.name(), dir);
    }

    cout << "Total files: " << (cnerr + cnold + cnnew) << '\n';
    if ( cnerr ) cout << "Errors occured: " << cnerr << '\n';
    cout << "Moved to new: " << cnnew << '\n';
    cout << "Moved to old: " << cnold << '\n';
}

void main_hash(ol::vstr & av)
{
    if ( av.size() != 1 ) throw "filename expected";
    string fn = av[0];

    cout << sam::qhash(fn) << '\n';
    cout << sam::fhash(fn) << '\n';
}

void index_addidx(ol::vstr & av)
{
    if ( av.size() < 2 ) throw "target_index index1 [index2] ...";
    string tar_idx = av[0];

    if ( !os::isFile(tar_idx) ) IndexFile().save(tar_idx);

    IndexFile fi(tar_idx);

    for ( int i = 1; i < (int)av.size(); i++ )
    {
        cout << "Adding " << av[i] << '\n';
        IndexFile add(av[i]);
        fi.insert(add.begin(), add.end());
    }

    fi.save();
    cout << "done\n";
}

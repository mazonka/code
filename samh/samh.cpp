
#include "samehf.h"
#include "copyfile.h"

bool inclDot = false;

void olmain(vector<string> & av)
{
    if ( av.size() < 2 )
    {
        std::cout << "samh, Oleg Mazonka, 2019, v0.1\n";
        std::cout << "Usage: longname, cutname\n";
///        std::cout << "Use @ (e.g. sameh@) to include '.' files and dirs\n";
///        std::cout << "sameh - find same files in the current dir\n";
///        std::cout << "copy D1 D2 D3 - copies files D1 to D2 moving from D3\n";
///        std::cout << "nocopy - checks files D1 by moving from D3 to D2\n";
///        std::cout << "nocopy - same as copy but does not copy files\n";
        std::cout << "longname - prints long file names\n";
        std::cout << "cutname ext pos size - cut long filenames\n";
        return;
    }

    string cmd = av[1];
    if ( !av.empty() ) av.erase(av.begin());
    if ( !av.empty() ) av.erase(av.begin());

    if ( cmd == "longname" )
    {
        void main_longname(vector<string> &);
        main_longname(av);
        return;
    }

    if ( cmd == "cutname" )
    {
        void main_cutname(vector<string> &);
        main_cutname(av);
        return;
    }

    throw "sam: unknown command [" + av[1] + "]";
}

// list file with full names longer than 100 or N bytes
void main_longname(vector<string> & args)
{
    std::cout << "Usage: longname [sizeB=100]\n";
    size_t size = 100;
    if ( args.size() > 0 ) size = std::stoi(args[0]);

    auto flist = sam::getListOfFiles(".", inclDot);

    int count = 0;
    size_t maxsz = 0;
    string maxnm;
    for ( const auto & entry : flist )
    {
        string name = entry.first.name();
        auto sz = name.size();
        if ( sz > maxsz ) { maxsz = sz; maxnm = name; }
        if ( name.size() > size )
        {
            count++;
            std::cout << name << '\n';
        }
    }

    if ( !count ) std::cout << "No long names found\n";
    else std::cout << "===\n";

    std::cout << "The longest is " << maxsz << '\n' << maxnm << '\n';
}

// cuts long file names
void main_cutname(vector<string> & args)
{
    if ( args.size() < 3 )
    {
        std::cout << "Usage: ext pos size\n";
        std::cout << "ext - file extention, '.' for no ext\n";
        std::cout << "pos - start position, cut before\n";
        std::cout << "size - max name length, 0 - for end\n";
        return;
    }

    string ext = "." + args[0];
    int pos = std::stoi(args[1]);
    int size = std::stoi(args[2]);

    auto flist = sam::getListOfFiles(".", inclDot);

    std::map<string, vector<sam::File>> dirs;
    for ( const auto & e : flist ) dirs[e.first.dname].push_back(e.first);

    std::map<sam::File, sam::File> oldnew;
    for ( const auto & x : dirs )
    {
        auto dir = x.first;
        auto vecf = x.second;

        vector<sam::File> cutname_filter(const vector<sam::File> &, string);
        auto vf = cutname_filter(vecf, ext);
        if ( vf.empty() ) continue;

        //cout << "DIR: " << dir << '\n';
        //for ( const auto & y : vf ) cout<< " [" << y.fname<<"]"; cout << '\n';

        auto nf = vf;
        void cutstrname(string & s, int, int, string);
        for ( auto & x : nf ) cutstrname(x.fname, pos, size, ext);

        // check uniqueness
        {
            std::map<string, int> m;
            for ( auto & x : nf )
            {
                if ( (++m[x.fname]) > 1 )
                    throw "Name [" + x.fname + "] in [" + dir + "] not unique";
            }
        }

        //for ( const auto & y : nf ) cout<< " [" << y.fname<<"]"; cout << '\n';
        for ( size_t i = 0; i < nf.size(); i++ )
            oldnew[vf[i]] = nf[i];
    }

    for ( auto x : oldnew )
    {
        auto of = x.first.name();
        auto nf = x.second.name();
        if ( nf == of ) continue;
        cout << of << " -> " << nf << '\n';
        os::FileSys::move(of, nf);
    }
}

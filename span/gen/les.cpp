#include <iostream>
#include <iomanip>
#include <fstream>
#include <exception>
#include <cstdlib>
#include <cctype>

#include "os.h"
#include "util.h"
#include "gl_utils.h"
#include "os_filesys.h"
#include "os_timer.h"
#include "gl_except.h"
#include "quest.h"

using std::cout;

extern int DIFFIC;
extern double ENFORCE_STRETCH;

Lesson::Lesson(string name, bool warn, bool trw_badtrn)
{
    diff = DIFFIC;

    if ( name.empty() )
    {
        auto vbase = find_srt_files(".");
        if ( vbase.size() > 1 ) throw 1;
        base = vbase[0];
    }
    else
    {
        int sz = (int)name.size();
        sz -= 4;

        if ( sz > 0 && name.substr(sz) == ".srt" )
            name = name.substr(0, sz);

        base = name;
    }

    if ( base.empty() ) throw 1; // "Cannot find lesson srt file";

    findvideo();

    srt = base + ".srt";
    lis = base + ".lis";
    trn = base + ".trn";

    load_srt(srt, items, this);

    // test that all items have hints
    {
        int empt = 0;

        for ( auto i : items )
            if ( i.etext.empty() ) ++empt;

        // if not items, try to load hint file
        if ( empt == (int)items.size() )
        {
            std::vector<Item> v;
            bool ishint = true;
            try {load_srt(srt + ".hint", v, this);}
            catch (...) { ishint = false; }

            if ( items.size() == v.size() )
                for ( int i = 0; i < empt; i++ )
                    items[i].etext = v[i].otext;

            else if (ishint) throw "Size mismatch in " + srt + ".hint";
        }

        if ( empt && warn )
            cout << "\nWARNING (" << base << "): some items do not have hints\n\n";
    }

    if ( !os::isFile(trn) ) init_hist(0);
    else load_hist(trn, items, 0, trw_badtrn);

    if ( !os::isFile(lis) ) init_hist(1);
    else load_hist(lis, items, 1, trw_badtrn);

    for ( auto & i : items )
    {
        if ( i.hist[0].size() != i.otext.size() )
        {
            cout << i.hist[0].size() << ' ' << i.otext.size() << '\n';
        }
        if ( i.hist[0].size() != i.otext.size() ) throw "File " + trn + " corrupted";
        if ( i.hist[1].size() != i.otext.size() ) throw "File " + lis + " corrupted";
    }

    for ( int quse = 0; quse < 2; quse++)
        rate0[quse] = t2r(rate(quse));

    // edulevel
    edu = Edu(base + ".lev", ENFORCE_STRETCH);
}

void Lesson::init_hist(int i)
{
    for ( auto & t : items )
    {
        Vvf & vvf = t.hist[i];
        init_history(vvf, t.otext);
    }
}

void Lesson::save(int i)
{
    string f[2] = { trn, lis };
    std::ofstream of(f[i].c_str(), std::ios::binary);

    // set defualt edu
    edu = Edu();
    updatestat(now());
    of << "# minfuture: " << st.minfuture << '\n';

    of << items.size() << '\n';
    for ( auto & t : items )
    {
        Vvf & vvf = t.hist[i];
        of << vvf.size() << '\n';
        for ( auto & v : vvf )
        {
            of << v.size() << ' ';
            for ( auto & q : v )
                of << std::setprecision(20) << q.t << ' ' << q.s << ' ';
            of << '\n';
        }
        of << '\n';
    }
}

void Lesson::saveqstat(string file)
{
    ///string file = "span.stat";
    if ( file.empty() ) return;

    int fs[2] {0, 0};
    {
        std::ifstream in(file);
        if (in) in >> fs[0] >> fs[1];
    }

    fs[0] += qs[0]; fs[1] += qs[1];

    int fss = fs[0] + fs[1];
    int qss = qs[0] + qs[1];
    int fsm = fs[0] - fs[1];
    int qsm = qs[0] - qs[1];

    auto z = [](int x, int y) -> double
    {
        return ((1.0 * x / y) - 0.75) * 400;
    };

    {
        std::ofstream of(file);
        if (fss) of << fs[0] << '\t' << fs[1] << '\t' << z(fsm, fss) << '\n';
        else of << "0 0 \n";
        if (qss) of << qs[0] << '\t' << qs[1] << '\t' << z(qsm, qss) << '\n';
    }
}

double Lesson::getqstat() const
{
    int qss = qs[0] + qs[1];
    if ( !qss ) return 0;
    return (qs[0] - qs[1]) * 1.0 / qss;
}

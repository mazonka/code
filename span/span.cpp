// Brain driller, Oleg Mazonka, 2017

// update 1710: idea using threshold
// prediction time is calculated according to the following rules
// if (fail) P -> P/4
// if (OK) P -> P+A+B
// A - is actual period from the last event
// B - is bonus B = max(0,0.5*(A-P))

// algorithm: (a1*1+a2*1/2+a3*1/4+...)/(1+1/2+1/4+...) * gk/N
// each printable letter has a history of HSZ(5)
// each leter has the rate as R=(a1*1+a2*1/2+a3*1/4+...)/(1+1/2+1/4+...)
// aN - average reply time (keep history up to 5, exponent maybe different)
// q=1/2 coeff
// item rate is average for all printable letters
// top item is found by R*gk
// N - number of items, gk<=N
// k - how long ago asked (number in queue start with 1), g - coefficient

// mkv, avi, mp4, mp3

// console code page: chcp 1252

#include <iostream>
#include <iomanip>
#include <fstream>
#include <exception>
#include <cstdlib>
#include <cctype>

#include "gen/os.h"
#include "gen/util.h"
#include "gen/gl_utils.h"
#include "gen/os_filesys.h"
#include "gen/os_timer.h"
#include "gen/gl_except.h"
#include "gen/quest.h"

const char * vlc_chk[] =
{
    "c:/Program Files/VideoLAN/VLC/vlc.exe",
    "c:/Program Files (x86)/VideoLAN/VLC/vlc.exe"
};
const char * video_chk[] = { ".avi", ".mkv", ".mp4", ".mp3", ".3gp" };

float esc_penalty = 20000;  // msec
double gcoeff = 0.2;      //  quest time line slope
double qcoeff = 0.7;      // decay letter history
int HIST = 15;           // times to keep per letter
int DIFFIC = 70;        // percent
double ENFORCE_STRETCH = 0;  // set stretch and ignore input file
bool quickexit = false;

using std::cout;

string find_vlc();
string g_vlc;

void watch(string f);
void trans(Lesson & les, int quse);
void drill(Lesson & les);
void step(Lesson & les);
void play(Item & it, string file);

int combine(int ac, const char ** av);
int tosrt(int ac, const char ** av);
int fixtrn(int ac, const char ** av);
int show(int ac, const char ** av, std::vector<Summary> *);
int mergef(int ac, const char ** av);
int dumpe(int ac, const char ** av);
int wordc(int ac, const char ** av);
int nowop(int ac, const char ** av);

void setcodepage();

void find_cfg(std::vector<string> & cfg);

int main(int argc1, const char * argv1[])
try
{
    cout << "\nBrain driller, Oleg Mazonka, 2016-2019, v180419\n";
    cout << "Usage: [option] [ srt_name | function ]\n";
    cout << "\tfunctions: -tosrt, -combine, -show, -fixtrn, -wc\n";
    cout << "\t           -merge, -dump, -any, -now, -testkey\n";
    cout << "\toptions: -stretch, -quit, -menu -r\n";
    cout << "\t\t -r = -stretch 1 -menu 1 -quit\n";
    cout << "\nTimestamp 2016 " << std::setprecision(10)
         << os::Timer::seconds2016() << '\n';


    std::vector<string> cfg;
    find_cfg(cfg);
    for ( int i = 1; i < argc1; i++ ) cfg.push_back(argv1[i]);


    g_vlc = find_vlc();
    if ( g_vlc.empty() )
    {
        cout << "ERROR: VLC not found in 'Program Files'\n";
        cout << "Searched in paths:\n";
        for ( auto i : vlc_chk ) cout << '\t' << i << '\n';
        cout << '\n';
    }
    else
        cout << "VLC = " << g_vlc << '\n';

    setcodepage();
    cout << '\n';

    string name;

    int ac = cfg.size();

    if ( ac > 0 )
    {

        const char ** argv = new const char *[ac + 1];
        for ( int i = 0; i < ac; i++ ) argv[i] = cfg[i].data();
        const char ** av = &argv[0];

        //cout << ac << " :"; for ( int i = 0; i < ac; i++ ) cout << ' ' << (&av[0])[i]; cout << '\n';
        //return 0;
        while ( ac > 0 )
        {
            string s = av[0];
            if (0);
            else if ( s == "-r")
            {
                quickexit = true;
                ENFORCE_STRETCH = 1;
                putcode('1');
                --ac; av = &av[1];
            }
            else if ( s == "-quit")
            {
                quickexit = true;
                --ac; av = &av[1];
            }
            else if ( s == "-testkey")
            {
		testkey();
		return 0;
            }
            else if ( s == "-stretch")
            {
                if (ac < 2 || !std::isdigit(av[1][0]) ) throw "need value";
                ENFORCE_STRETCH = std::atof(av[1]);
                ac -= 2; av = &av[2];
            }
            else if ( s == "-menu")
            {
                if (ac < 2 || !std::isdigit(av[1][0]) ) throw "need value";
                putcode(av[1][0]);
                ac -= 2; av = &av[2];
            }
            else if ( s == "-any" || s[0] != '-' )
            {
                name = s;
                --ac; av = &av[1];
            }
            else if ( s == "-combine" ) return combine(ac, av);
            else if ( s == "-tosrt" ) return tosrt(ac, av);
            else if ( s == "-show" ) return show(ac, av, nullptr);
            else if ( s == "-now" ) return nowop(ac, av);
            else if ( s == "-fixtrn" ) return fixtrn(ac, av);
            else if ( s == "-merge") return mergef(ac, av);
            else if ( s == "-wc") return wordc(ac, av);
            else if ( s == "-dump") return dumpe(ac, av);
            else throw "Bad option [" + s + "]";

        }

        if (name == "-any")
        {
            // run show to collect lessons
            std::vector<Summary> sum;
            show(0, nullptr, &sum);
            double nw = now();

            if (sum.empty()) throw "No lessons";

            // select one
            auto tm = [nw](double t) -> double { return (t > 1e12) ? nw : t; };
            /*///
            for (int i = 0; i < (int)sum.size(); i++)
            {
                LessonStat & st = sum[i].st;
                double t = tm(st.minfuture);
                if (st.notasked > 0) t = nw;
                if (st.badans > 0) t = 0;
                if ( t < nw ) t = nw - (nw - t) * (st.outside + st.badans);
                st.minfuture = t;
            }

            int j = 0;
            double jt = tm(sum[0].st.minfuture);
            for (int i = 1; i < (int)sum.size(); i++)
            {
                double t = tm(sum[i].st.minfuture);
                if (t < jt) { j = i; jt = t; }
            }
            */
            int j = 0;
            for (int i = 1; i < (int)sum.size(); i++)
            {
                LessonStat & sj = sum[j].st;
                LessonStat & si = sum[i].st;
                ///double tj = tm(sj.minfuture);
                ///double ti = tm(si.minfuture);
                if ( si < sj ) j = i;
            }

            // go to dir
            const Summary & s = sum[j];
            // continue with new name
            name = s.full;
        }

    } // ac > 0

    Lesson les(name, true, true);

    cout << "Lesson " << les.base << ", " << les.items.size() << " items\n";
    cout << "   Video               : " << (les.vid.empty() ? "No" : les.vid) << '\n';
    cout << "   Srt                 : " << (les.srt.empty() ? "No" : les.srt) << '\n';
    cout << "   Translating history : " << (os::isFile(les.trn) ? "Yes" : "No") << '\n';
    cout << "   Listening history   : " << (os::isFile(les.lis) ? "Yes" : "No") << '\n';

start:
    cout << '\n';

    double flue1 = t2r(les.rate(0));
    double flue2 = t2r(les.rate(1));
    les.updatestat(now());

    cout << " (1) Refresh ( " << les.st.str() << " )\n";
    cout << " (2) Listen ( fluency " << flue2 << " )\n";
    cout << " (3) Step through ( fluency " << flue1 << " )\n";
    cout << " (4) Watch the video\n";
    cout << " (5) Old training\n";
    //cout << " (5) Practice ( fluency " << flue1 << " )\n";
    //cout << " Level " << les.diff << "%, (8) to decrease or (9) to increase\n";
    cout << " (0) Exit\n";
    cout << "enter 1-4, or 0 : ";
code:
    unsigned char k = getcode(0);

    if ( k == '0' )
    {
        if ( name.empty() ) name = les.srt;
        cout << "bye [" << name << "]\n"; return 0;
    }

    if ( k < '1' || k > '9' ) goto code;

    cout << k << '\n';

    if ( k == '4' ) watch(les.vid);
    if ( k == '2' ) trans(les, 1);
    if ( k == '5' ) trans(les, 0);
    if ( k == '3' ) step(les);
    if ( k == '1' ) drill(les);
    //if ( k == '9' ) { les.diff += 10; if (les.diff > 100) les.diff = 100; }
    //if ( k == '8' ) { les.diff -= 10; if (les.diff < 0) les.diff = 0; }

    goto start;

}
catch (int e)
{
    //try {show(0, nullptr, nullptr);}
    try {nowop(0, nullptr);}
    catch (...) {}
    string dir = os::FileSys::cwd().str();
    if (dir.size() > 20) dir = "... " + dir.substr(dir.size() - 20);
    cout << "\nError: (" << e << ") No (unique) srt file in [ " << dir << " ]\n";
    return 5;
}
catch (const char * e)
{
    cout << "\nError: " << e << "\n";
    return 4;
}
catch (string e)
{
    cout << "\nError: " << e << "\n";
    return 4;
}
catch (std::exception & e)
{
    cout << "\nError (C++): " << e.what() << "\n";
    return 5;
}
catch (...)
{
    cout << "Error unknown\n";
    return 2;
}



string find_vlc()
{
    for ( auto s : vlc_chk )
    {
        //cout << s << '\n';
        if ( os::isFile(s) ) return s;
    }

    return "";
}

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

void watch(string f)
{
    if ( f.empty() )
    {
        cout << "\nVideo file is not available\n";
        return;
    }

    string cmd = "\"" + g_vlc + "\" " + f;
    //cout << cmd << '\n';
    system(cmd.c_str());
    return;

}

void step(Lesson & les)
{
    cout << "\nStepping through " << les.items.size() << " items";
    cout << "\nPress Enter to go to the next, Esc to exit, or Space for video\n\n";

    for ( auto & i : les.items )
    {
        cout << "[ " << i.from << " --> " << i.to << " ]\n";

        if ( !i.etext.empty() )
        {
            cout << i.etext << '\n';
            getcode(0);
        }

        cout << i.otext << '\n';

get:
        uchar v[3] = { uchar(13), uchar(27), uchar(32) };
        uchar k = getoneof(3, v);

        if ( k == 32 )
        {
            if ( !les.vid.empty() ) play(i, les.vid);
            else cout << "No video available\n";
            goto get;
        }

        i.seennow();

        if ( k == 27 ) break;
    }

    les.save(0);
    //if ( les.vid.empty() );

}

void play(Item & it, string f)
{
    string cmd = "\"" + g_vlc + "\" " + f;
    cmd += " --start-time=" + gl::tos(it.from);
    cmd += " --stop-time=" + gl::tos(it.to);
    cmd += " vlc://quit";
    //cout << cmd << '\n';
    system(cmd.c_str());
}

void print_etext(string t)
{
    int w = os::termSize().first - WINDOW_MARGIN;
    int sz = (int)t.size();
    for ( int i = 0, j = 0; i < sz; i++, j++ )
    {
        if ( ( j > w && t[i] == ' ' ) || t[i] == '\n' )
        {
            j = 0;
            cout << '\n';
        }
        else
            cout << t[i];
    }

    cout << "\n\n";
}

void trans(Lesson & les, int quse)
{
    if ( quse < 0 || quse > 1 ) throw "bad quse";

    if ( quse )
    {
        if ( les.vid.empty() )
        {
            cout << "\nListening training cannot be done because video file is not available\n";
            return;
        }
    }

    cout << "\nType the answers, Esc for help\n";
    cout << "At the end\n\tSpace - play video\n\t"
         "Enter - go to the next item\n\t0 - back to the main menu\n\n";

    while (1)
    {
        int idx = select_item(les, quse);
        Item & it = les.items[idx];

        if (quse == 0)
            ///cout << it.etext << '\n';
            print_etext(it.etext);
        else
            play(it, les.vid);

        int itm_rt1 = t2ri(it.rate(quse));
        int les_rt1 = t2ri(les.rate(quse));

        Quest qu(&les, idx, quse);
        qu.ask(true);

        int itm_rt2 = t2ri(it.rate(quse));
        int les_rt2 = t2ri(les.rate(quse));

        cout << "\nItem fluency : " << itm_rt1 << " -> " << itm_rt2 << '\n';
        cout << "Lesson fluency : " << les_rt1 ;
        if ( les_rt1 != les_rt2 ) cout << " -> " << les_rt2;

        les.updatestat(now());
        cout << " [" << les.st.str(false) << "]";
        cout << '\n';

        string spc = ", Space - video";
        if ( it.to == 0 || les.vid.empty() ) spc = "";
        cout << "\n(Enter - next, 0 - exit" << spc << ") ";

        if ( quickexit && les.st.ok() ) putcode('0', '0');

        while (1)
        {
            uchar k = getcode(0);

            if ( k == '0' ) { cout << "\n\n"; les.save(quse); return; }
            if ( k == ' ' && !les.vid.empty() ) play(it, les.vid);
            if ( k == uchar(13) ) { cout << "\n\n"; break; }
        }

        les.que[quse].push_back(idx);
    }
}

void drill(Lesson & les)
{
    // check all letters have time stamp
    if (!les.updatestat(now()))
    {
        cout << "Please use 'Step through' first to initiate this function";
        getcode(0);
        return;
    }

    auto out = [](LessonStat st)
    {
        cout << '\n' << st.str() << "\n\n";
    };

    out(les.st);

    cout << "\nType the answers, Esc for help\n";
    cout << "At the end\n\tSpace - play video\n\t"
         "Enter - go to the next item\n\t0 - back to the main menu\n\n";

    if ( '0' == getcode(0) ) return;

    unsigned ix = unsigned(now());
    if ( quickexit == true ) ix = 0;

    // always start 0 if there are
    if ( les.st.notasked ) ix = 0;

    if ( ix == 0 ) --ix;

    for (;;)
    {
        int conh = os::termSize().second;
        if ( conh < 1 )
            cout << "\n\n\n\n\n\n\n" << "\n\n\n\n\n\n\n"
                 << "\n\n\n\n\n\n\n" << "\n\n\n\n\n\n\n";
        else
            for ( int i = 0; i < conh; i++ ) cout << '\n';

        les.updatestat(now());
        out(les.st);

        int idx = select_tran(les, ix, 0);
        Item & it = les.items[idx];

        print_etext(it.etext);

        int itm_rt1 = t2ri(it.rate(0));
        int les_rt1 = t2ri(les.rate(0));

        Quest qu(&les, idx, 0);
        qu.ask(true);

        int itm_rt2 = t2ri(it.rate(0));
        int les_rt2 = t2ri(les.rate(0));

        cout << "\nItem fluency : " << itm_rt1 << " -> " << itm_rt2 << '\n';
        cout << "Lesson fluency : " << les_rt1 ;
        if ( les_rt1 != les_rt2 ) cout << " -> " << les_rt2;
        cout << '\n';

        les.updatestat(now());
        out(les.st);

        if ( quickexit && les.st.ok() ) putcode('0', '0');

        string spc = ", Space - video";
        if ( it.to == 0 || les.vid.empty() ) spc = "";
        cout << "\n(Enter - next, 0 - exit" << spc << ") ";

        while (1)
        {
            uchar k = getcode(0);

            if ( k == '0' ) { cout << "\n\n"; les.save(0); return; }
            if ( k == ' ' && !les.vid.empty() ) play(it, les.vid);
            if ( k == uchar(13) ) { cout << "\n\n"; break; }
        }

        les.que[0].push_back(idx);
    }
}


int combine(int ac, const char ** av)
{
    if ( ac < 4 )
    {
        cout << "Use: -combine in_esp.srt in_eng.srt out.srt\n";
        return 1;
    }

    string fesp = av[1];
    string feng = av[2];
    string fout = av[3];

    cout << "Input esp : " << fesp << '\n';
    cout << "Input eng : " << feng << '\n';
    cout << "Output    : " << fout << '\n';

    Lesson esp(fesp, false, true);
    Lesson eng(feng, false, true);

    if ( esp.items.size() != eng.items.size() )
        throw "Item size mismatch esp="
        + gl::tos(esp.items.size())
        + " eng=" + gl::tos(eng.items.size());

    int sz = (int)esp.items.size();
    for ( int i = 0; i < sz; i++ )
        esp.items[i].etext = eng.items[i].otext;

    save_srt(fout, esp.items);

    return 0;
}

int tosrt(int ac, const char ** av)
{
    if ( ac < 3 )
    {
        cout << "Use: -tosrt in.srt out.srt\n";
        return 1;
    }

    string fesp = av[1];
    string fout = av[2];

    cout << "Input  : " << fesp << '\n';
    cout << "Output : " << fout << '\n';

    Lesson esp(fesp, true, true);
    save_srt(fout, esp.items);

    return 0;
}

void Lesson::findvideo()
{
    const int DEPTH = 7;
    string b = base;

    while (1)
    {
        for ( auto i : video_chk )
        {
            string n = b + i;
            for ( int k = 0; k < DEPTH; k++ )
            {
                if ( os::isFile(n) ) { vid = n; return; }
                n = "../" + n;
            }
        }

        auto p = b.rfind('-');
        if ( p == string::npos ) return;

        b = b.substr(0, p);
    }
}

void output_les_stat(LessonStat st, double flue)
{
    auto s = [](double x) -> string
    {
        string r = gl::tos(x);
        while (r.size() < 8) r = r + " ";
        return r;
    };

    cout << s(int(flue)) << s(st.badans);
    cout << s(st.outside + 0 * st.soon); // do not scare
    cout << s(st.notasked) << s(st.inside);

    string f = st.future_str(8);
    ///while (f.size() < 8) f = f + " ";
    cout << f;
}

void showr(
    const string & d,
    Show & data,
    int lev,
    std::vector<Summary> * sum,
    WordCount * wc
)
{
    std::vector<string> v = find_srt_files(d);

    Show srts;

    for ( auto b : v )
    {
        if ( !b.empty() )
        {
            string n = b;
            if ( d != "." ) b = d + '/' + b;
            Lesson les(b, false, false);
            double flue1 = t2r(les.rate(0));
            //double flue2 = t2r(les.rate(1));
            les.updatestat(now());

            *wc += les.wordcount();

            if ( lev > 0 )
            {
                output_les_stat(les.st, flue1);
                cout << ' ' << b << '\n';
            }

            srts.add(les.st, flue1);
            if (sum) sum->push_back(Summary(d, n, b, les.st));
        }
    }

    os::Dir dir = os::FileSys::readDir(d);

    for ( auto i : dir.dirs )
    {
        Show sd;
        string dd = i;

        if ( d != "." ) dd = d + "/" + i;

        showr(dd, sd, lev - 1, sum, wc);

        if ( lev == 1 && sd.size() )
        {
            output_les_stat(sd.st, sd.flue);
            cout << ' ' << dd << '\n';
        }

        data += sd;
    }

    data += srts;
}

int show(int ac, const char ** av, std::vector<Summary> * sum)
{
    int level = 1;
    if ( ac > 1 ) level = std::atoi(av[1]);

    cout <<  "\nFluency Bad     Need    New     Old     When     Name\n";
    cout <<    "=====================================================\n";

    Show data;

    WordCount wc;
    showr(".", data, level, sum, &wc);
    cout <<    "-----------------------------------------------------\n";
    output_les_stat(data.st, data.flue);
    cout << '\n';

    cout << '\n' << wc.str() << '\n';

    return 0;
}

int fixtrn(int ac, const char ** av)
{
    if ( ac < 2 )
    {
        cout << "Use: -fixtrn lesson\n";
        return 1;
    }

    string fesp = av[1];

    cout << "Lesson : " << fesp << '\n';

    Lesson les(fesp, true, false);

    les.save(0);

    return 0;
}


void setcodepage()
{
    unsigned x = os::codepage();

    g_codepage = x;
    if ( x == 1252 ) std::cout << "Codepage: ANSI (Spanish)\n";
    else if (x == 1253) std::cout << "Codepage: ANSI - Greek\n";
    else if (x == 1251)
    {
        const char * p = std::getenv("KEYB");
        string q = "ru"; // default
        if ( p ) q = p;
        string s = "Codepage: ANSI - Russian (";
        if ( !p ) s += "Env KEYB not set => ru";
        else if ( q == "ru" || q == "us" ) s += q;
        else s += "Warning: KEYB value can be only 'ru' or 'us'";
        s += ")\n";
        std::cout << s ;
        g_keyb = q;
    }
    else
    {
        g_codepage = 0;
        std::cout << "\nWARNING: Codepage " << x << " is not recognised\n";
        std::cout << "\tBy default using chcp 1252\n";
        std::cout << "\tThis version implemets 1252 and 1253\n";
    }
}

int mergef(int ac, const char ** av)
{
    if ( ac < 4 )
    {
        cout << "Use: -merge in1.srt in2.srt out.srt\n";
        return 1;
    }

    string f1 = av[1];
    string f2 = av[2];
    string fo = av[3];

    cout << "Input 1 : " << f1 << '\n';
    cout << "Input 2 : " << f2 << '\n';
    cout << "Output  : " << fo << '\n';

    Lesson s1(f1, true, true);
    Lesson s2(f2, true, true);

    std::vector<Item> v;
    v.insert(v.end(), s1.items.begin(), s1.items.end());
    v.insert(v.end(), s2.items.begin(), s2.items.end());

    save_srt(fo, v);

    Lesson so(fo, true, true);
    so.items = v;
    so.save(0);
    so.save(1);

    return 0;
}

int dumpe(int ac, const char ** av)
{
    if (ac < 3)
    {
        cout << "Use: -dump in.srt out.txt [separator]\n";
        cout << "separator is optional file containing separator between items\n";
        return 1;
    }

    string fi = av[1];
    string fo = av[2];
    string s = "";
    cout << "Input     : " << fi << '\n';
    cout << "Output    : " << fo << '\n';

    if (ac > 3)
    {
        s = gl::file2str(av[3]);
        cout << "Separator : " << av[3] << '\n';
    }

    Lesson si(fi, true, true);

    std::ofstream of(fo);

    for (auto x : si.items)
    {
        of << x.otext << s;
    }

    return 0;
}

int wordc(int ac, const char ** av)
{
    if (ac < 2)
    {
        cout << "Use: -wc {srt or txt file}\n";
        cout << "word count statistics\n";
        return 1;
    }

    string fi = av[1];

    bool srt = false;

    try
    {
        int sz = fi.size();
        if ( sz < 5 ) throw 1;
        if ( fi.substr(sz - 4) == ".srt" ) srt = true;
    }
    catch (int)
    {
        throw "Input file must be .srt or .txt";
    }

    cout << "Input     : " << fi << '\n';

    WordCount wc;
    if (srt)
    {
        Lesson si(fi, true, true);
        wc = si.wordcount();
    }
    else
    {
        wc = wordcount(gl::file2str(fi));
    }

    cout << wc.str() << '\n';

    return 0;
}

void find_cfg(std::vector<string> & cfg)
{
    const int DEPTH = 7;
    string f;

    string n = "span.cfg";
    for ( int k = 0; k < DEPTH; k++ )
    {
        if ( os::isFile(n) ) { f = n; break; }
        n = "../" + n;
    }

    if ( f.empty() ) return;

    std::cout << "Found config: " << f << '\n';

    std::ifstream in(f);

    for ( string s; in >> s; ) cfg.push_back(s);
}


double load_minfut(string f)
{
    f += ".trn";
    std::ifstream in(f);
    if ( !in ) return 1;
    char c;
    in.get(c);
    if ( c != '#' ) return 2;
    string s;
    double r = 0;
    in >> s >> r;
    return r;
}

void nowopr(
    const string & d,
    Show & data,
    int lev
)
{
    std::vector<string> v = find_srt_files(d);

    Show srts;

    for ( auto b : v )
    {
        if ( !b.empty() )
        {
            string n = b;
            if ( d != "." ) b = d + '/' + b;
            LessonStat st { 0, 0, 0, 0, -1, 0};

            st.minfuture = 2 * now() - load_minfut(b);

            if ( lev > 0 )
            {
                ///output_les_stat(st, st.minfuture/1000000);
                if ( st.minfuture > now() )
                    cout << st.future_str(5) << ' ' << b << '\n';
            }

            srts.add(st, -1);
        }
    }

    os::Dir dir = os::FileSys::readDir(d);

    for ( auto i : dir.dirs )
    {
        Show sd;
        string dd = i;

        if ( d != "." ) dd = d + "/" + i;

        nowopr(dd, sd, lev - 1);

        if ( lev == 1 && sd.size() )
        {
            ///output_les_stat(sd.st, sd.flue);
            if ( sd.st.minfuture > now() )
                cout << sd.st.future_str(5) << ' ' << dd << '\n';
        }

        data += sd;
    }

    data += srts;
}

int nowop(int ac, const char ** av)
{
    int level = 20;
    if ( ac > 1 ) level = std::atoi(av[1]);

    cout <<  "\n      Name   now: " << now() << "\n";
    cout <<    "=====================================================\n";

    Show data;

    nowopr(".", data, level);
    cout <<    "-----------------------------------------------------\n";

    return 0;
}

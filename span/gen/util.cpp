
#include <iostream>
#include <fstream>
#include <cctype>
#include <cmath>

#include "os.h"
#include "os_timer.h"
#include "os_filesys.h"
#include "gl_utils.h"

#include "util.h"
#include "les.h"

using std::cout;

unsigned g_codepage = 0;
string g_keyb;

void eatSpace(string & s)
{
    while ( s.size() && (s[s.size() - 1] == '\r' || s[s.size() - 1] == '\n') )
        s.erase(s.size() - 1, 1);

    while ( s.size() && std::isspace(uchar(s[0])) ) s = s.substr(1);
}

void eatPatern(string & s, string fr, string to)
{
    auto i = s.find(fr);
    auto j = s.rfind(to);
    if ( i == string::npos || j == string::npos ) return;
    s = s.substr(0, i) + s.substr(j + to.size());
}

uchar getoneof(int sz, uchar * x)
{
    while (1)
    {
        uchar k = getcode(0);
        for ( int i = 0; i < sz; i++ )
            if ( k == x[i] ) return k;
    }
}

std::vector<char> getcode_input;
void putcode (char c) { getcode_input.push_back(c); }
unsigned char getcode(double time_window)
{
    if ( !getcode_input.empty() )
    {
        char r = *getcode_input.rbegin();
        getcode_input.pop_back();
        return r;
    }

    os::Timer timer;
    unsigned char c = 0;
    ///int cntr = 1;
    while (1)
    {
        c = (unsigned char)os::kbhit();
        if (c) break;
        os::sleep(1);

        auto tw = time_window - timer.get();
        if ( time_window > 0 && tw < 0 )
            return '\0';
    }

    return c;
}

/*///
unsigned char getcode(char pr, int delay)
{
    if ( !getcode_input.empty() )
    {
        char r = *getcode_input.rbegin();
        getcode_input.pop_back();
        return r;
    }

    os::Timer timer;
    auto tget = timer.get();
    decltype(tget) wait_msec = unsigned(delay);
    delay *= 1000;


    auto show = [](char c)
    {
        cout << c << std::flush;
        os::sleep(100);
        cout << "\x8" << std::flush;
    };

    char prmt[4] = { ' ', ' ', ' ', ' ' };
    prmt[2] = prmt[3] = pr;
    char flsh = '#';
    if ( pr == '.' ) flsh = ':';

    unsigned char c = 0;
    int cntr = 1;
    while (1)
    {
        c = (unsigned char)os::kbhit();
        if (c) break;
        tget = timer.get();
        if ( tget > wait_msec ) break;
        if ( cntr == 1 ) show(flsh);
        show(prmt[(++cntr) % 4]);
        //cout << ' ' << (wait_msec - tget);
        //cout << "    \r" << std::flush;
    }

    return c;
}

*/

double load_srt_time(std::istream & in, bool eol)
{
    double x = 0;

    string s;
    std::getline(in, s, ':'); // hours
    x += gl::toi(s) * 60.0 * 60;
    std::getline(in, s, ':'); // mins
    x += gl::toi(s) * 60.0;
    std::getline(in, s, ','); // sec
    x += gl::toi(s);

    if (eol) std::getline(in, s); // msec
    else std::getline(in, s, ' ');
    x += gl::toi(s) * 0.001;

    return x;
}

void load_raw_stream(std::ifstream & in, std::vector<Item> & items, const Lesson * les)
{
    int cntr = 0;
    while (in)
    {
        string line;
        std::getline(in, line);
        eatSpace(line);
        if ( line.empty() ) break;

        ++cntr;
        Item item;
        item.n = ++cntr;

        item.from = item.to = 0;
        item.otext += line + '\n';
        items.push_back(item);
    }

    for ( size_t i = 0; i < items.size(); i++ )
    {
        string line;
        std::getline(in, line);
        eatSpace(line);
        if ( !in || line.empty() ) throw "Not enough translations";
        items[i].etext += line + '\n';
    }

    string line;
    std::getline(in, line);
    eatSpace(line);
    if ( !line.empty() ) throw "Too many translations";
}

void load_raw_file(string file, std::vector<Item> & items, const Lesson * les)
{
    std::ifstream in(file);
    if ( !in ) throw "Cannot open " + file;

    while (in)
    {
        std::vector<Item> v;
        load_raw_stream(in, v, les);
        items.insert(items.end(), v.begin(), v.end());
    }
}

void load_srt(string file, std::vector<Item> & items, const Lesson * les)
{
    std::ifstream in(file);

    if ( !in ) throw "Cannot open " + file;

    int cntr = 0;
    while (in)
    {
        string line;

        // get N
        std::getline(in, line);
        eatSpace(line);
        if ( line.empty() ) continue;

        if ( !cntr && !std::isdigit(line[0]) && line[0] != '@' )
        {
            //cout << "Loading raw data as srt file\n";
            in.close();
            load_raw_file(file, items, les);
            return;
        }

        ++cntr;
        Item item;
        item.n = ++cntr;

        if ( line[0] != '@' )
        {
            item.n = gl::toi(line);

            // get time --> time
            item.from = load_srt_time(in, false);
            std::getline(in, line, ' ');
            item.to = load_srt_time(in, true);
        }
        else
        {
            item.from = item.to = 0;
        }

        // get text
        while (in)
        {
            std::getline(in, line);
            eatSpace(line);
            if ( line.empty() ) break;
            eatPatern(line, "{\\an", "}");
            if ( line.empty() ) continue;
            if ( line[0] == '#')
                item.etext += line.substr(1) + '\n';
            else
                item.otext += line + '\n';
        }

        items.push_back(item);
    }
}

uchar askable_1252(uchar c)
{
    if ( c >= 'A' && c <= 'Z' ) return c - 'A' + 'a';
    if ( c >= 'a' && c <= 'z' ) return c;
    if ( c >= '0' && c <= '9' ) return c;
    if ( c == uchar(193) ) return 'a'; // Á
    if ( c == uchar(225) ) return 'a'; // á
    if ( c == uchar(201) ) return 'e'; // É
    if ( c == uchar(233) ) return 'e'; // é
    if ( c == uchar(205) ) return 'i'; // Í
    if ( c == uchar(237) ) return 'i'; // í
    if ( c == uchar(209) ) return 'n'; // Ñ
    if ( c == uchar(241) ) return 'n'; // ñ
    if ( c == uchar(211) ) return 'o'; // Ó
    if ( c == uchar(243) ) return 'o'; // ó
    if ( c == uchar(218) ) return 'u'; // Ú
    if ( c == uchar(250) ) return 'u'; // ú
    if ( c == uchar(220) ) return 'u'; // Ü
    if ( c == uchar(252) ) return 'u'; // ü
    //if( c == uchar(171) ) return '<'; // «
    //if( c == uchar(187) ) return '>'; // »
    //if( c == uchar(191) ) return '?'; // ¿
    //if( c == uchar(161) ) return '!'; // ¡

    return uchar(0);
}

uchar askable_1253(uchar c)
{
    if ( c == uchar(193) || c == uchar(225) ) return 'a';
    if ( c == uchar(194) || c == uchar(226) ) return 'b';
    if ( c == uchar(195) || c == uchar(227) ) return 'g';
    if ( c == uchar(196) || c == uchar(228) ) return 'd';
    if ( c == uchar(197) || c == uchar(229) ) return 'e';
    if ( c == uchar(198) || c == uchar(230) ) return 'z';
    if ( c == uchar(199) || c == uchar(231) ) return 'h';
    if ( c == uchar(200) || c == uchar(232) ) return 'q';
    if ( c == uchar(201) || c == uchar(233) ) return 'i';
    if ( c == uchar(202) || c == uchar(234) ) return 'k';
    if ( c == uchar(203) || c == uchar(235) ) return 'l';
    if ( c == uchar(204) || c == uchar(236) ) return 'm';
    if ( c == uchar(205) || c == uchar(237) ) return 'n';
    if ( c == uchar(206) || c == uchar(238) ) return 'c';
    if ( c == uchar(207) || c == uchar(239) ) return 'o';
    if ( c == uchar(208) || c == uchar(240) ) return 'p';
    if ( c == uchar(209) || c == uchar(241) ) return 'r';
    if ( c == uchar(211) || c == uchar(243) ) return 's';
    if ( c == uchar(212) || c == uchar(244) ) return 't';
    if ( c == uchar(213) || c == uchar(245) ) return 'u';
    if ( c == uchar(214) || c == uchar(246) ) return 'f';
    if ( c == uchar(215) || c == uchar(247) ) return 'x';
    if ( c == uchar(216) || c == uchar(248) ) return 'y';
    if ( c == uchar(217) || c == uchar(249) ) return 'w';

    if ( c == uchar(162) || c == uchar(220) ) return 'a';
    if ( c == uchar(184) || c == uchar(221) ) return 'e';
    if ( c == uchar(185) || c == uchar(222) ) return 'h';
    if ( c == uchar(186) || c == uchar(223) ) return 'i';
    if ( c == uchar(192) || c == uchar(218) ) return 'i';
    if ( c == uchar(250) ) return 'i';
    if ( c == uchar(188) || c == uchar(252) ) return 'o';
    if ( c == uchar(242) ) return 's';
    if ( c == uchar(190) || c == uchar(253) ) return 'u';
    if ( c == uchar(219) || c == uchar(251) ) return 'u';
    if ( c == uchar(224) ) return 'u';
    if ( c == uchar(191) || c == uchar(254) ) return 'w';

    return uchar(0);
}

// Style US Keyboard
uchar askable_1251us(uchar c)
{
    if ( c == uchar(192) || c == uchar(224) ) return 'a'; // ??
    if ( c == uchar(193) || c == uchar(225) ) return 'b'; // ??
    if ( c == uchar(194) || c == uchar(226) ) return 'v'; // ??
    if ( c == uchar(195) || c == uchar(227) ) return 'g'; // ??
    if ( c == uchar(196) || c == uchar(228) ) return 'd'; // ??
    if ( c == uchar(197) || c == uchar(229) ) return 'e'; // ??
    if ( c == uchar(198) || c == uchar(230) ) return 'x'; // ??
    if ( c == uchar(199) || c == uchar(231) ) return 'z'; // ??
    if ( c == uchar(200) || c == uchar(232) ) return 'i'; // ??
    if ( c == uchar(201) || c == uchar(233) ) return 'j'; // ??
    if ( c == uchar(202) || c == uchar(234) ) return 'k'; // ??
    if ( c == uchar(203) || c == uchar(235) ) return 'l'; // ??
    if ( c == uchar(204) || c == uchar(236) ) return 'm'; // ??
    if ( c == uchar(205) || c == uchar(237) ) return 'n'; // ??
    if ( c == uchar(206) || c == uchar(238) ) return 'o'; // ??
    if ( c == uchar(207) || c == uchar(239) ) return 'p'; // ??
    if ( c == uchar(208) || c == uchar(240) ) return 'r'; // ??
    if ( c == uchar(209) || c == uchar(241) ) return 's'; // ??
    if ( c == uchar(210) || c == uchar(242) ) return 't'; // ??
    if ( c == uchar(211) || c == uchar(243) ) return 'u'; // ??
    if ( c == uchar(212) || c == uchar(244) ) return 'f'; // ??
    if ( c == uchar(213) || c == uchar(245) ) return 'h'; // ??
    if ( c == uchar(214) || c == uchar(246) ) return 'c'; // ??
    if ( c == uchar(215) || c == uchar(247) ) return 'q'; // ??
    if ( c == uchar(216) || c == uchar(248) ) return 'w'; // ??
    if ( c == uchar(217) || c == uchar(249) ) return 'w'; // ??
    if ( c == uchar(218) || c == uchar(250) ) return '\''; // ??
    if ( c == uchar(219) || c == uchar(251) ) return 'y'; // ??
    if ( c == uchar(220) || c == uchar(252) ) return '\''; // ??
    if ( c == uchar(221) || c == uchar(253) ) return ';'; // ??
    if ( c == uchar(222) || c == uchar(254) ) return ','; // ??
    if ( c == uchar(223) || c == uchar(255) ) return '.'; // ??

    if ( c == uchar(168) || c == uchar(184) ) return 'e'; // ??

    return uchar(0);
}

// Style Russian Keyboard
uchar askable_1251ru(uchar c)
{
    if ( c == uchar(192) || c == uchar(224) ) return 'f'; // ??
    if ( c == uchar(193) || c == uchar(225) ) return ','; // ??
    if ( c == uchar(194) || c == uchar(226) ) return 'd'; // ??
    if ( c == uchar(195) || c == uchar(227) ) return 'u'; // ??
    if ( c == uchar(196) || c == uchar(228) ) return 'l'; // ??
    if ( c == uchar(197) || c == uchar(229) ) return 't'; // ??
    if ( c == uchar(198) || c == uchar(230) ) return ';'; // ??
    if ( c == uchar(199) || c == uchar(231) ) return 'p'; // ??
    if ( c == uchar(200) || c == uchar(232) ) return 'b'; // ??
    if ( c == uchar(201) || c == uchar(233) ) return 'q'; // ??
    if ( c == uchar(202) || c == uchar(234) ) return 'r'; // ??
    if ( c == uchar(203) || c == uchar(235) ) return 'k'; // ??
    if ( c == uchar(204) || c == uchar(236) ) return 'v'; // ??
    if ( c == uchar(205) || c == uchar(237) ) return 'y'; // ??
    if ( c == uchar(206) || c == uchar(238) ) return 'j'; // ??
    if ( c == uchar(207) || c == uchar(239) ) return 'g'; // ??
    if ( c == uchar(208) || c == uchar(240) ) return 'h'; // ??
    if ( c == uchar(209) || c == uchar(241) ) return 'c'; // ??
    if ( c == uchar(210) || c == uchar(242) ) return 'n'; // ??
    if ( c == uchar(211) || c == uchar(243) ) return 'e'; // ??
    if ( c == uchar(212) || c == uchar(244) ) return 'a'; // ??
    if ( c == uchar(213) || c == uchar(245) ) return '['; // ??
    if ( c == uchar(214) || c == uchar(246) ) return 'w'; // ??
    if ( c == uchar(215) || c == uchar(247) ) return 'x'; // ??
    if ( c == uchar(216) || c == uchar(248) ) return 'i'; // ??
    if ( c == uchar(217) || c == uchar(249) ) return 'o'; // ??
    if ( c == uchar(218) || c == uchar(250) ) return ']'; // ??
    if ( c == uchar(219) || c == uchar(251) ) return 's'; // ??
    if ( c == uchar(220) || c == uchar(252) ) return 'm'; // ??
    if ( c == uchar(221) || c == uchar(253) ) return '\''; // ??
    if ( c == uchar(222) || c == uchar(254) ) return '.'; // ??
    if ( c == uchar(223) || c == uchar(255) ) return 'z'; // ??
    if ( c == uchar(168) || c == uchar(184) ) return '`'; // ??

    return uchar(0);
}

uchar askable(uchar c)
{
    if ( g_codepage == 1252 ) return askable_1252(c);
    if ( g_codepage == 1253 ) return askable_1253(c);
    if ( g_codepage == 1251 )
    {
        if ( g_keyb == "us" ) return askable_1251us(c);
        return askable_1251ru(c);
    }

    return askable_1252(c);
}


void load_hist(string file, std::vector<Item> & items, int q, bool trw)
{
    std::ifstream in(file);

    // first ignore #
    {
        char c;
        in.get(c);
        if ( c != '#' ) in.putback(c);
        else
        {
            string line;
            std::getline(in, line);
        }
    }


    int z1 = 0;
    in >> z1;

    if ( items.size() - z1 )
    {
        if ( trw ) throw "Bad " + file + ": - size mismatch (1) use -fixtrn";
        if ( z1 > (int)items.size() ) z1 = (int)items.size();
    }

    for ( int i = 0; i < z1; i++ )
    {
        Item & it = items[i];
        int z2 = 0;
        in >> z2;

        Vvf * vvf = nullptr, vvf_dummy;

        if ( it.otext.size() - z2 )
        {
            if ( trw ) throw "Bad " + file + ": - size mismatch (2) use -fixtrn";
            vvf = &vvf_dummy;
            init_history(it.hist[q], it.otext);
        }
        else
            vvf = &it.hist[q];

        for ( int j = 0; j < z2; j++ )
        {
            int z3 = 0;
            in >> z3;
            Vf vf;
            for ( int k = 0; k < z3; k++ )
            {
                float x = -2, y = -2;
                in >> x >> y;
                vf.push_back(Point(x, y));
            }
            vvf->push_back(vf);
        }
    }

    for ( int i = z1; i < (int)items.size(); i++ )
    {
        Item & it = items[i];
        init_history(it.hist[q], it.otext);
    }
}

double Item::rate(int q, int j)
{
    if ( j < 0 && j >= (int)otext.size() ) return -1;
    Vf & vf = hist[q][j];
    int sz = (int)vf.size();
    double x = 0, w = 1, y = 0;
    for ( int i = 0; i < sz; i++ )
    {
        x += w * vf[sz - i - 1].t;
        y += w;
        w *= qcoeff;
    }
    return y > 0 ? x / y : 0;
}

double Item::rate(int q)
{
    Vvf & vvf = hist[q];
    int sz = (int)vvf.size();

    double s = 0, x, m = 0;
    int c = 0;
    for ( int i = 0; i < sz; i++ )
        if ( askable(otext[i]) )
        {
            x = rate(q, i);
            s += x;
            m = (m < x ? x : m);
            c++;
        }

    double a = c ? s / c : 0;

    return (a + m) / 2;
}


int select_item(Lesson & les, int quse)
{
    int sz = (int)les.items.size();

    double mx = -1;
    int idx = -1;

    for ( int i = 0; i < sz; i++ )
    {
        int k = 1000000; // position in que

        int qz = (int)les.que[quse].size();
        for ( int j = 0; j < qz; j++ )
            if ( les.que[quse][j] == i ) k = qz - j;

        double gkN = gcoeff * k;
        if ( gkN > 1 ) gkN = 1;

        double rt = les.items[i].rate(quse);
        double x = rt * gkN;

        if ( x > mx )
        {
            idx = i;
            mx = x;
        }
    }

    return idx;
}


double Lesson::rate(int quse)
{
    double s = 0, y, m = 0;
    for ( auto & x : items )
    {
        y = x.rate(quse);
        s += y;
        m = m < y ? y : m;
    }

    return 0.5 * ( m + (s / items.size()) );
}

string form_time(double x)
{
    string r;
    int yh = int(x / 60 / 60);
    string z = gl::tos(yh); if ( z.size() < 2 ) z = "0" + z;
    r = z + ":";

    int ym = int(x / 60) - yh * 60;
    z = gl::tos(ym); if ( z.size() < 2 ) z = "0" + z;
    r += z + ":";

    int ys = int(x) - yh * 60 * 60 - ym * 60;
    z = gl::tos(ys); if ( z.size() < 2 ) z = "0" + z;
    r += z + ",";

    int ms = int(1000 * (x - int(x)));
    z = gl::tos(ms); while ( z.size() < 3 ) z = "0" + z;
    r += z;

    return r;
}

string hash_convert(string s)
{
    if ( s.empty() ) return s;
    if ( s[s.size() - 1] == '\n' ) s = s.substr(0, s.size() - 1);
    s = "\n" + s;
    gl::replaceAll(s, "\n", "&_N_&");
    gl::replaceAll(s, "&_N_&", "\n#");
    s = s.substr(1) + '\n';
    return s;
}

void save_srt(string file, std::vector<Item> & items)
{
    std::ofstream of(file);

    if ( !of ) throw "Cannot open " + file;

    for ( int i = 0; i < (int)items.size(); i++ )
    {
        of << (i + 1) << '\n';
        Item & it = items[i];
        of << form_time(it.from) << " --> " << form_time(it.to) << '\n';

        of << it.otext;
        string et = hash_convert(it.etext);
        of << et;

        of << '\n';
    }

}

double now()
{
    return os::Timer::seconds2016();
}

int select_tran(Lesson & les, unsigned & ix, int reclev)
{
    int sz = (int)les.items.size();
    if (++reclev > 10) return ix % sz; // tried 10 times recursively
    ix = (ix + 1) % sz;
    //int ix = int(now()) % sz;

    // use non-random with preference from the beginning
    //ix = 0;

    // try this strategy - making it more difficult
    //if ( les.st.notasked ) return select_item(les, 0);

    if ( les.st.badans )
///        while (1)
        for ( int i = 1; i < sz; i++ )
        {
            Item & it = les.items[ix];
            if ( !it.st.badans.empty() ) return ix;
            ix = (ix + 1) % sz;
        }

    if ( les.st.outside )
        for ( int i = 1; i < sz; i++ )
        {
            Item & it = les.items[ix];
            if ( !it.st.outside.empty() ) return ix;
            ix = (ix + 1) % sz;
        }

    if ( les.st.notasked )
        for ( int i = 1; i < sz; i++ )
        {
            Item & it = les.items[ix];
            if ( !it.st.notasked.empty() ) return ix;
            ix = (ix + 1) % sz;
        }

    //return select_item(les, 0);
    // try smaller stretch
    double ostr = les.edu.stretch;
    for ( ; les.edu.stretch > 0.1; les.edu.stretch *= 0.8)
    {
        les.updatestat(now());
        if ( les.st.outside )
        {
            int r = select_tran(les, ix, reclev);
            les.edu.stretch = ostr;
            les.updatestat(now());
            return r;
        }
    }

    les.edu.stretch = ostr;
    les.updatestat(now());
    return select_item(les, 0);
}


string LessonStat::future_str()
{
    if ( badans || outside || notasked ) return "NOW";
    double t = minfuture - now();
    if ( t < 0 ) return "now";

    if ( t < 60 ) return gl::tos(int(t + 0.5)) + " s";
    t /= 60;

    if ( t < 60 ) return gl::tos(int(t + 0.5)) + " m";
    t /= 60;

    if ( t < 24 ) return gl::tos(int(t + 0.5)) + " h";
    t /= 24;

    if ( t < 30 ) return gl::tos(int(t + 0.5)) + " d";
    if ( t < 365 ) return gl::tos(int(t / 30 + 0.5)) + " M";
    t /= 365;

    return gl::tos(int(t + 0.5)) + " Y";
}

string LessonStat::future_str(int sz)
{
    string f = future_str();
    while ((int)f.size() < sz) f += " ";
    return f;
}

string LessonStat::str(bool longer)
{
    string r = "bad:out:new:old:strength = ";
    if ( !longer ) r = "";
    r = r
        + gl::tos(badans)
        + ':' + gl::tos(outside) // + '/' + gl::tos(soon)
        + ':' + gl::tos(notasked)
        + ':' + gl::tos(inside)
        + ':' + future_str();

    return r;
}

std::vector<string> find_srt_files(string path)
{
    string base;
    std::vector<string> r;

    os::Dir dir = os::FileSys::readDir(path);

    for ( auto i : dir.files )
    {
        string n = i.first;
        if ( n.size() < 5 ) continue;
        string ext = n.substr(n.size() - 4);
        if ( ext == ".srt" )
            r.push_back(n.substr(0, n.size() - 4));
    }

    if ( r.empty() ) r.push_back("");

    return r;
}


void init_history(Vvf & vvf, const string & ot)
{
    for ( size_t j = 0; j < ot.size(); j++ )
    {
        Vf vf;
        Point p(esc_penalty, -1);
        vf.push_back(p);
        if ( !askable(ot[j]) )
            vf[0].t = -1;
        else
            while ( (int)vf.size() < HIST ) vf.push_back(p);
        vvf.push_back(vf);
    }
}

void Show::add(LessonStat s, double f)
{
    if ( f < flue ) flue = f;

    st.badans += s.badans;
    st.outside += s.outside;
    st.notasked += s.notasked;
    st.inside += s.inside;

    if ( st.minfuture > s.minfuture ) st.minfuture = s.minfuture;
}

int Show::size() const
{
    return st.badans + st.outside + st.notasked + st.inside;
}

Edu::Edu(string file, double enf_st) : Edu()
{
    if (os::isFile(file))
    {
        std::ifstream in(file);
        if (!in) throw "cannot open file " + file;

        while (in)
        {
            string k;
            in >> k;
            if (k.empty() || !in ) break;
            else if (k == "level") in >> level;
            else if (k == "stretch") in >> stretch;
            else if (k == "shrink") in >> shrinkf;
            else if (k == "badkey") in >> bkpen;
            else if (k == "bonus") in >> boncon;
            else if (k == "prndelay") in >> prndelay;
            else if (!k.empty() && k[0] == '#' )
            {
                string s;
                in >> s;
            }
            else
                throw "Value [" + k + "] in file [" + file
                + "] is not supported\n"
                + "\tuse one of: {level, stretch, shrink, "
                "badkey, bonus, prndelay}\n"
                + "\tlevel 0 is a special (original) algorithm "
                "when all edu values are ignored";
        }
    }

    if (enf_st > 0) stretch = enf_st;
}

WordCount Lesson::wordcount() const
{
    WordCount w;

    for ( const auto & i : items )
        w += i.wordcount();

    return w;
}

WordCount wordcount(string s)
{
    WordCount r;
    if ( s.empty() ) return r;

    s += '\0';
    int sz = (int)s.size();

    bool c = false;
    string w;
    for ( int i = 0; i < sz; i++ )
    {
        bool a = ( askable(s[i]) != uchar(0) );
        if (0);
        else if ( c && a ) { w += s[i]; }
        else if ( c && !a ) { r.uniq.insert(w); r.total++; w = ""; c = !c; }
        else if ( !c && a ) { w += s[i]; c = !c; }
        else if ( !c && !a ) {}
    }

    return r;
}

WordCount Item::wordcount() const { return ::wordcount(otext); }

void WordCount::operator+=(const WordCount & x)
{
    total += x.total;
    uniq.insert(x.uniq.begin(), x.uniq.end());
}

string WordCount::str() const
{
    double wcoeff = 0;
    int usz = (int)uniq.size();
    if ( usz ) wcoeff = 1.0 * total / usz;

    // original value 0.0165443
    double comp = std::pow(total, 1 - std::log(total) / 60);

    if ( comp > 1 )
    {
        comp = usz / comp;
        comp = (comp - 1) * 100;
        comp = int((comp + 1000) + 0.5) - 1000.0;
    }

    std::ostringstream os;

    os << "Words: " << total
       << "  Unique: " << usz
       << "  W/U: " << wcoeff
       << "  Complexity: " << comp;

    return os.str();
}

void testkey()
{
    while (1)
    {
        uchar k = getcode(0);
        if ( k == uchar(13) ) break;
        for ( uchar i = 0; i < uchar(255); i++ )
        {
            uchar a = askable(i);
            if ( a && a == k ) std::cout << i << std::flush;
        }
    }
}

void keymap()
{
    for ( int j = 0; j <= 255; j++ )
    {
	uchar i = uchar(j);
        uchar a = askable(i);
        if ( !a ) continue;
        std::cout << i << ' ' << a << " | ";
    }
    std::cout<<'\n';
}

bool LessonStat::use_minfuture {false};
bool LessonStat::use_maxnum {false};
bool LessonStat::use_new {false};

bool LessonStat::operator<(const LessonStat & x) const
{
    if ( use_new )
    {
        if ( notasked < x.notasked ) return false;
        if ( notasked > x.notasked ) return true;
    }

    if ( badans > x.badans ) return true;
    if ( badans < x.badans ) return false;

    if ( outside && !x.outside ) return true;
    if ( !outside && x.outside ) return false;

    double nw = now();
    auto tm = [nw](double t) -> double { return (t > 1e12) ? nw : t; };
    double tt = tm(minfuture);
    double tx = tm(x.minfuture);

    if ( use_minfuture ) return tt < tx;

    if ( outside && x.outside )
    {
        if ( use_maxnum )
        {
            if ( outside < x.outside ) return false;
            if ( outside > x.outside ) return true;
        }
        else
        {
            if ( outside < x.outside ) return true;
            if ( outside > x.outside ) return false;
        }
        return tt < tx;
    }

    // no outside for both
    if ( !notasked && !x.notasked ) return notasked < x.notasked;
    if ( notasked && !x.notasked ) return true;
    if ( !notasked && x.notasked ) return false;

    // no notasked
    return tt < tx;
}

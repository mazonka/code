#pragma once

#include <vector>
#include <string>
#include <set>

using std::string;

const int WINDOW_MARGIN = 15;

extern unsigned g_codepage;  // current codepage - 0 for unknown
extern string g_keyb;  // current keyboard for Russian "ru/us"

extern double qcoeff;           // decay letter history
extern float esc_penalty;       // msec
extern int HIST;            // times to keep per letter
extern double gcoeff;           //  quest time line slope

inline double t2d(double x) { return 100 * (esc_penalty / x - 1); }
inline double t2r(double x) { return 0.01 * int(100 * t2d(x)); }
inline int t2ri(double x) { return int(0.5 + t2r(x)); }

struct Point
{
    float t ; // time delay
    double s; // stamp
    Point(double at, double ap): t((float)at), s(ap) {}
    Point(): t(-1), s(-1) {}
};

typedef std::vector<Point> Vf;
typedef std::vector<Vf> Vvf;

struct ItemStat
{
    std::vector<int> badans, outside, notasked, inside, soon;
    double minfuture; // absolute seconds
};

struct Lesson;

struct Edu
{
    double level; // time threshold in seconds; how quickly you must respond
    double stretch; // strength of repetition remembering; 5 means roughly ask after 5 times longer
    double shrinkf; // include near future; 0.8 means reduce stretch by 80% when asking
    double bkpen; // bad key penatly (sec);
    double boncon; // constant time (sec) added as a bonus for correct answer
    unsigned long prndelay; // delay between printing non-asking letters

    Edu() : level(5), stretch(3), shrinkf(0.2),
        bkpen(3), boncon(0), prndelay(50) {}

    Edu(string file, double enf_st);

    Edu shrink() const { Edu r(*this); r.stretch *= r.shrinkf; return r; }
};

struct WordCount
{
    std::set<string> uniq;
    int total = 0;
    void operator += (const WordCount &);
    string str() const;
};

struct Item
{
    string otext;
    string etext;
    int n;
    double from, to;
    Vvf hist[2];

    ItemStat st;

    double rate(int q, int i);
    double rate(int q);

    void seennow();
    bool updatestat(double tim, Edu edu);
    static double getperiod(Vf & vf, Edu edu);
    static double getperedu(Vf & vf, Edu edu);

    WordCount wordcount() const;
};

struct LessonStat
{
    int badans, outside, notasked, inside;
    double minfuture; // absolute seconds
    int soon;
    string future_str();
    string future_str(int sz);
    string str(bool longer = true);
    bool ok() const { return !(badans || outside || notasked); }

    bool operator<(const LessonStat & x) const;
};

struct Lesson
{
        string base;
        string vid;
        string srt;
        string lis;
        string trn;
        std::vector<Item> items;
        std::vector<int> que[2];
        double rate0[2];
        int diff;
        LessonStat st;
        Edu edu;

        Lesson(string name, bool warn_nohint, bool trw_badtrn);
        void save(int);
        void init_hist(int);
        double rate(int quse);
        bool updatestat(double tim);

        WordCount wordcount() const;

    private:
        Lesson(const Lesson &);
        void findvideo();
};

typedef unsigned char uchar;
unsigned char getcode(double time_window);
///unsigned char getcode(char pr = '.', int = -1);
void putcode(char c);
inline void putcode(char a, char b) { putcode(a); putcode(b); }

uchar getoneof(int sz, uchar * x);
void load_srt(string file, std::vector<Item> & items, const Lesson * les);
void load_raw_file(string file, std::vector<Item> & items, const Lesson * les);
void save_srt(string file, std::vector<Item> & items);
int select_item(Lesson & les, int quse);
int select_tran(Lesson & les, unsigned & hint, int reclev);
void load_hist(string file, std::vector<Item> & items, int, bool trw);
uchar askable(uchar c);
double now();
WordCount wordcount(string s);
void testkey();

// returns at least one empty element;
std::vector<string> find_srt_files(string dir);

void init_history(Vvf & vvf, const string & ot);

struct Show
{
    LessonStat st;
    double flue;

    void add(LessonStat, double);
    void operator+=(const Show & s) { add(s.st, s.flue); }

    Show(): flue(1000) { LessonStat s { 0, 0, 0, 0, 1e12 }; st = s; } // workaround

    int size() const;
};

struct Summary
{
    string dir;
    string name;
    string full;
    LessonStat st;
    Summary() {}
    Summary(string d, string n, string f, LessonStat s) : dir(d), name(n), full(f), st(s) {}
};



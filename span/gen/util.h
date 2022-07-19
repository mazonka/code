#pragma once

#include <vector>
#include <string>
#include <set>

#include "les.h"

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
void keymap();

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

    bool operator<(const Summary & x) const { return st < x.st; }
};



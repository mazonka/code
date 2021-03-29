#pragma once

#include <vector>
#include <string>
#include <set>

using std::string;


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

    static bool use_minfuture, use_maxnum, use_new;

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
        int qs[2] {0, 0}; // [0/1] right/wrong keyb answers

        Lesson(string name, bool warn_nohint, bool trw_badtrn);
        void save(int);
        void init_hist(int);
        double rate(int quse);
        bool updatestat(double tim);

        WordCount wordcount() const;

        void saveqstat(string file);
        double getqstat() const;
        void updateqstat(int q[2]) { qs[0] += q[0]; qs[1] += q[1]; }

    private:
        Lesson(const Lesson &);
        void findvideo();
};

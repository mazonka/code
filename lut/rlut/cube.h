#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>

using std::string;
using std::pair;

using vd = std::vector<double>;
using vdd = std::vector< pair<double, double> >;
using vvd = std::vector< std::vector<double> >;
using ull = unsigned long long;
using vu = std::vector<ull>;


void mul(vd & v, double k);
void add(vd & v, const vd & u);

const int OPREC = 18;

#define ERR (string()+__FILE__+":"+std::to_string(__LINE__))

struct Point
{
    unsigned lev = 0;
    vu coords;

    void normalize();
    bool operator<(const Point & p) const;
    bool operator==(const Point & p) const;
    Point sub() const; // make sub-point
};

class Cube
{
        Point base;

        Cube * parent = nullptr;
        std::vector<Cube *> kids;

    public:

        Cube(Point p, Cube * r) : base(p), parent(r) {}
        ~Cube() { for (auto x : kids) delete x; }

        const Point & point() const { return base; }
        void addKid(Cube * k) { kids.push_back(k); k->parent = this; }
        std::vector<Point> listVerts() const;
        Cube * findKid(Point p) const;
        std::vector<Cube *> getKids() { return kids; }

    private:
        Cube(const Cube &) = delete;
        void operator=(const Cube &) = delete;
};


// gcc 4.9.2 cygwin does not have to_string
#ifdef __GNUC__
#ifdef __CYGWIN__
#ifdef __CYGWIN32__
namespace std
{
template <class T> inline string to_string(T x)
{ std::ostringstream os; os << x; return os.str(); }
} // std
#endif
#endif
#endif

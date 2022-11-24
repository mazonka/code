#include <string>
#include <vector>
#include <map>

using std::string;
using std::pair;

using vd = std::vector<double>;
using vdd = std::vector< pair<double, double> >;
using vvd = std::vector< std::vector<double> >;
using vu = std::vector<unsigned>;

using ull = unsigned long long;

struct Point
{
    unsigned lev = 0;
    vu coords;

    void normalize();
    bool operator<(const Point & p) const;
};

class Cube
{
        Point base;

        Cube * parent = nullptr;
        std::vector<Cube *> kids;

    public:

        Cube(Point p, Cube * r) : base(p), parent(r) {}

        Point point() const { return base; }
        ~Cube() { for (auto x : kids) delete x; }
        void addKid(Cube * k) { kids.push_back(k); k->parent = this; }
        std::vector<Point> listVerts() const;

    private:
        Cube(const Cube &) = delete;
        void operator=(const Cube &) = delete;
};

struct PtsClass
{
    vd precX, precY;
    vvd points;
};

struct Config
{
    int dimX, dimY;
    vdd rangeX, rangeY;

    std::vector<PtsClass> ptsClasses;
};

class Lut
{
        Cube * root = nullptr;

        std::map<Point, vd> lut;

    public:
        Config cfg;

    public:
        Lut(string file);
        ~Lut() { delete root; }

        pair<Point, Cube *> getSubCube(vd point, Cube * parent);
        Cube * addCube(Point p, Cube * parent);
        bool checkPrecX(Cube * cb, const vd & pt, const PtsClass & pc);
        bool checkPrecY(Cube * cb, const vd & pt, const PtsClass & pc);

    private:
        void insertLut(Point p);
        vd translate(Point p);
};

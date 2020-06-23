#pragma once

#include <vector>
#include <string>
#include <map>
#include <ostream>
#include <istream>

struct Fcoord
{
    double v = 0;
    bool fixed = false;

    Fcoord(double x = 0, bool fix = false) : v(x), fixed(fix) {}
    ///Fcoord() : v(0), fixed(false) {}
    ///std::string str() const;
    void save(std::ostream & o);
    void load(std::istream & o);
};

inline Fcoord fix(double x) { return Fcoord {x, true}; }

///extern Fcoord X;

struct Line
{
    std::string name;
};

struct Span
{
    int i1, i2;
    void operator=(Line);
    void operator=(double);
};

struct Distance
{
    Span s;
    double d;
    ///void save(std::ostream & o, int ind);
};

struct Point
{
        Fcoord x, y, z;
        int index;
        std::string name;

        Point(Fcoord a = 0, Fcoord b = 0, Fcoord c = fix(0), std::string n = "")
            : x(a), y(b), z(c), name(n) { reg();  }
        Point(std::string n, Fcoord a = 0, Fcoord b = 0, Fcoord c = fix(0))
            : Point(a, b, c, n) {}
        Point(std::istream & in);

        Span operator-(const Point & p) const;

        void save(std::ostream & o, int ind);

    private:
        void reg();
};

struct Edge
{
    Span s;
    Line ln;
    void draw(const std::vector<Point> & points);
    ///void save(std::ostream & o, int ind);
};

class Model
{
        std::string name;

        std::vector<Model> submodels;
        std::vector<Point> points;
        std::vector<Distance> distances;
        std::vector<Edge> edges;

        std::map<std::string, int> pnames;

    public:
        Model(std::string n);
        ~Model();

        int addPoint(Point p);
        void addDist(Distance d) { distances.push_back(d); }
        void addEdge(Edge e) { edges.push_back(e); }

        void draw();
        double calc(); // returns loss

        template <class T> double loss(T x);

        void save(std::string name);
        void load(std::string name);

    private:
        std::vector<int> idxs; // indices to point coordinates
        // they are exactly # of points * 3; -1 means fixed
        std::vector<double> loss_vals; // values for loss function
        void cook_idxs();
        void commit_vals();
        Span readSpan(std::istream & o);
        void save(std::ostream & o, int ind);
        void load(std::istream & o);
};

extern Model g_model_main;
extern Model * g_model_current;

void model();

inline void save(std::string name) { g_model_current -> save(name); }
inline void load(std::string name) { g_model_current -> load(name); }
inline void draw() { g_model_current -> draw(); }
inline void calc() { g_model_current -> calc(); }

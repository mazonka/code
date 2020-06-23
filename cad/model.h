#pragma once

#include <vector>
#include <string>

struct Fcoord
{
    double v;
    enum class Typ { fixed, about, unknown } typ;

    Fcoord(double x) : v(x), typ(Typ::fixed) {}
    Fcoord() : v(0), typ(Typ::unknown) {}
    std::string str() const;
};

extern Fcoord X;

struct Line
{
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
};

struct Point
{
        Fcoord x, y, z;
        int index;

        Point(Fcoord a = X, Fcoord b = X, Fcoord c = 0) : x(a), y(b), z(c) { reg();  }

        Span operator-(const Point & p) const;

    private:
        void reg();
};

struct Edge
{
    Span s;
    Line ln;
    void draw(const std::vector<Point> & points);
};

class Model
{
        std::vector<Model> submodels;
        std::vector<Point> points;
        std::vector<Distance> distances;
        std::vector<Edge> edges;

    public:
        Model();
        ~Model();

        int addPoint(Point p) { points.push_back(p); return int(points.size() - 1); }
        void addDist(Distance d) { distances.push_back(d); }
        void addEdge(Edge e) { edges.push_back(e); }

        void draw();
        double calc(); // returns loss function
};

extern Model g_model_main;
extern Model * g_model_current;

void draw();
void model();


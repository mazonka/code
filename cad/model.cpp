#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "bad.h"
#include "model.h"

///Fcoord X;
std::vector<Model *> g_model_stack;
Model * g_model_current = nullptr;
Model g_model_main("main");

using std::string;
using std::cout;

Model::Model(std::string n): name(n)
{
    g_model_stack.push_back(this);
    g_model_current = this;
}

Model::~Model()
{
    if (this != g_model_current) return; // submodel

    g_model_stack.pop_back();
    if (g_model_stack.empty())
    {
        g_model_current = nullptr;
        return;
    }
    g_model_current = *g_model_stack.rbegin();
    g_model_current->submodels.push_back(*this);
}

void Point::reg()
{
    index = g_model_current->addPoint(*this);
    x = y = z = Fcoord(0, true);
}

void Model::draw()
{
    for ( auto & a : submodels ) a.draw();

    std::map<string, std::vector<Edge> > me;
    for ( auto e : edges ) me[e.ln.name].push_back(e);

    for ( const auto & pe : me )
    {
        auto n = pe.first;
        std::ofstream of("plot/" + n + ".dat");
        for ( auto e : pe.second ) e.draw(of, points);
    }
}

void Span::operator=(double a)
{
    g_model_current->addDist(Distance { *this, a });
}


void Span::operator=(Line ln)
{
    g_model_current->addEdge(Edge { *this, ln });
}

Span Point::operator-(const Point & p) const
{
    return Span { index, p.index };
}

void Edge::draw(std::ostream & o, const std::vector<Point> & points) const
{
    auto dr = [&o](Point p) -> string
    {
        o << p.x.v << '\t' << p.y.v << '\t' << p.z.v;
        return "";
    };

    o << dr(points[s.i1]) << '\n';
    o << dr(points[s.i2]) << "\n\n";
}


void Model::save(std::string fname)
{
    std::ofstream of(fname);
    if ( !of ) throw "Cannot open file: " + fname;
    save(of, 0);
}

void Model::load(std::string fname)
{
    std::ifstream in(fname);
    if ( !in ) throw "Cannot open file: " + fname;
    string s;

    in >> s;
    if ( s != "model" ) throw "loading model error [" + s + "] " + LN;

    load(in);
}

void Model::save(std::ostream & o, int ind)
{
    string sd(ind, ' ');
    o << sd << "model " << name << "\n" << sd << "{\n";

    for ( auto p : points ) p.save(o, ind);

    for ( auto d : distances )
    {
        o << sd << "dist " << d.d
          << ' ' << points[d.s.i1].name
          << ' ' << points[d.s.i2].name << '\n';
    }

    for ( auto e : edges )
    {
        o << sd << "edge " << e.ln.name
          << ' ' << points[e.s.i1].name
          << ' ' << points[e.s.i2].name << '\n';
    }

    for ( auto m : submodels ) m.save(o, ind + 4);

    o << sd << "}\n";
}

void Model::load(std::istream & in)
{
    submodels.clear();
    points.clear();
    distances.clear();
    edges.clear();
    pnames.clear();

    string s;
    in >> name >> s;
    if ( s != "{" ) throw "loading model error [" + s + "] " + LN;

    while ( in >> s )
    {
        if ( s == "}" ) break;

        else if ( s == "pt" ) addPoint(Point(in));

        else if ( s == "dist" )
        {
            double a; in >> a;
            addDist(Distance { readSpan(in), a });
        }

        else if ( s == "edge" )
        {
            string a; in >> a;
            addEdge(Edge { readSpan(in), Line{a} });
        }

        else if ( s == "model" )
        {
            Model m("");
            m.load(in);
        }

        else throw "loading model error [" + s + "] " + LN;
    }
}

void Fcoord::save(std::ostream & o)
{
    o << ' ';
    if ( fixed ) o << "fix ";
    o << v;
}

void Fcoord::load(std::istream & in)
{
    string s;
    in >> s;
    if ( s == "fix" ) { fixed = true; in >> s; }

    v = std::stod(s);
}

void Point::save(std::ostream & o, int ind)
{
    string d(ind, ' ');
    BAD(name.empty());
    o << d << "pt " << name;
    x.save(o);
    y.save(o);
    z.save(o);
    o << '\n';
}

int Model::addPoint(Point p)
{
    int idx = int(points.size());
    p.index = idx;

    // create point name
    if ( p.name.empty() ) p.name = "p" + std::to_string(idx);

    // register name
    auto i = pnames.find(p.name);
    if ( i != pnames.end() )
        throw "Point [" + p.name + "] already defined in model [" + name + "]";

    pnames[p.name] = idx;

    // add
    points.push_back(p);
    return idx;
}

Point::Point(std::istream & in)
{
    in >> name;
    x.load(in); y.load(in); z.load(in);
}

Span Model::readSpan(std::istream & in)
{
    string n1, n2;
    in >> n1 >> n2;
    return Span {pnames[n1], pnames[n2]};
}

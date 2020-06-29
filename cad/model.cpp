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

Model::Model(std::string n): name(n), fname(n)
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
    Streams o;
    draw(o);
}

void Model::draw(Streams & o)
{
    std::map<string, std::vector<Edge> > me;
    for ( auto e : edges ) me[e.ln.name].push_back(e);

    for ( const auto & pe : me )
    {
        auto n = pe.first;
        ///std::ofstream of("plot/" + n + ".dat");
        auto i = o.i.find(n);
        if ( i == o.i.end() )
            cout << "Warning: edge type [" << n << "] is unknown\n";
        else
            for ( auto e : pe.second ) e.draw(o.o[i->second], points);
    }

    // write labels
    {
        int cntr = 0;
        for ( auto p : points )
        {
            o.labels << "set label " << (++cntr) << " \"" << p.name
                     << "\" at " << p.x.v << "," << p.y.v << "," << p.z.v << "\n";
        }
    }

    for ( auto & a : submodels ) a.draw(o);
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

void Model::load(std::string fn)
{
    fname = fn;
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
    if (!points.empty()) o << '\n';

    for ( auto d : distances )
    {
        o << sd << "dist " << d.d
          << ' ' << points[d.s.i1].name
          << ' ' << points[d.s.i2].name << '\n';
    }
    if (!distances.empty())o << '\n';

    for ( auto e : edges )
    {
        o << sd << "edge " << e.ln.name
          << ' ' << points[e.s.i1].name
          << ' ' << points[e.s.i2].name << '\n';
    }
    if (!edges.empty()) o << '\n';

    for ( auto m : submodels )
    {
        o << '\n';
        m.save(o, ind + 2);
    }

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
        if ( s == "#" ) { std::getline(in, s); }

        else if ( s == "}" ) break;

        else if ( s == "pt" ) addPoint(Point(in, false));
        else if ( s == "pz" ) addPoint(Point(in, true));

        else if ( s == "dist" )
        {
            double x; in >> x;
            addDist(Distance { readSpan(in), x });
        }

        else if ( s == "edge" )
        {
            string a; in >> a;
            addEdge(Edge { readSpan(in), Line{a} });
        }

        else if ( s == "span" )
        {
            string a; in >> a;
            double x; in >> x;
            auto sp = readSpan(in);
            addEdge(Edge { sp, Line{a} });
            addDist(Distance { sp, x });
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
    bool t3d = ( !z.fixed || z.v > 0 || z.v < 0 );
    string ptname = t3d ? "pz" : "pt";
    o << d << ptname << " " << name;
    x.save(o);
    y.save(o);
    if ( t3d ) z.save(o);
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

Point::Point(std::istream & in, bool t3d)
{
    in >> name;
    x.load(in); y.load(in);
    if (t3d) z.load(in);
    else z = Fcoord {0, true};
}

Span Model::readSpan(std::istream & in)
{
    string n1, n2;
    in >> n1 >> n2;

    auto i1 = pnames.find(n1);
    auto i2 = pnames.find(n2);

    if ( i1 == pnames.end() ) throw "No point for name [" + n1 + "]";
    if ( i2 == pnames.end() ) throw "No point for name [" + n2 + "]";

    return Span {i1->second, i2->second};
}

Streams::Streams()
{
    int c = 0; for ( std::string s : names )
    { o.emplace_back("o/" + s + ".dat"); i[s] = c++; }
    if ( !labels )
        throw "Cannot open files for writing"
        " in [" + std::string(dir) + "]";
}

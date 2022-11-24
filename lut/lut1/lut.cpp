#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>

#include "lut.h"
#include "f.h"

using std::cout;

namespace lut_local
{
struct GenDim { double min, max; int n; };

string readline(std::istream & in, bool eatempty);
void add_points(string cfg_line, vvd & v);
void load_from_file(string file, vvd & v);
void add_gen_point(vvd & v, const std::vector<GenDim> & vgd, vd pt);
} // lut_local

using namespace lut_local;
using istr = std::istringstream;

Lut::Lut(string file)
{
    // load config
    {
        std::ifstream in(file + ".cfg");
        if ( !in ) throw "Cannot open file [" + file + "]";

        // read dimensions
        auto line = readline(in, true);

        if ( !(istr(line) >> cfg.dimX >> cfg.dimY) )
            throw file + ": reading dimensions failed";

        // read ranges
        line = readline(in, true);
        {
            istr is(line);
            for (int i = 0; i < cfg.dimX; i++)
            {
                double mn, mx;
                is >> mn >> mx;
                cfg.rangeX.push_back(std::make_pair(mn, mx));
            }
            if ( !is ) throw file + ": reading X ranges failed";
        }

        line = readline(in, true);
        {
            istr is(line);
            for (int i = 0; i < cfg.dimY; i++)
            {
                double mn, mx;
                is >> mn >> mx;
                cfg.rangeY.push_back(std::make_pair(mn, mx));
            }
            if (!is) throw file + ": reading Y ranges failed";
        }

        // read point classes
        while (1)
        {
            PtsClass pc;

            // read precicions
            line = readline(in, true);
            {
                istr is(line);
                double x = 100;
                for (; is >> x;) pc.precX.push_back(x);
                while ((int)pc.precX.size() < cfg.dimX) pc.precX.push_back(x);
            }
            if (!in) break;

            line = readline(in, true);
            {
                istr is(line);
                double x = 100;
                for (; is >> x;) pc.precY.push_back(x);
                while ((int)pc.precY.size() < cfg.dimY) pc.precY.push_back(x);
            }

            // read points
            line = readline(in, true);
            while (!line.empty())
            {
                add_points(line, pc.points);
                line = readline(in, false);
            }

            cfg.ptsClasses.push_back(pc);
        }

    }
}

string lut_local::readline(std::istream & in, bool eatempty)
{
    string line;
    std::getline(in, line);
    if (!in) return "";
    if (line.empty())
    {
        if (!eatempty) return "";
        return readline(in, eatempty);
    }
    if (line[0] != '#') return line;
    return readline(in, eatempty);
}

void lut_local::load_from_file(string file, vvd & v)
{
    std::ifstream in(file);
    if (!in) throw "Cannot open [" + file + "]";

    for (string line; std::getline(in, line);)
    {
        istr is(line);
        vd a;
        for (double x; is >> x;) a.push_back(x);
        v.push_back(a);
    }
}

void lut_local::add_points(string cfg_line, vvd & v)
{
    istr is(cfg_line);

    string s;
    is >> s;

    if (!is || s.empty()) throw "bad points line [" + cfg_line + "]";

    if (!std::isdigit(s[0]))
    {
        load_from_file(s, v);
        return;
    }

    cout << "Generating points [ " << cfg_line << " ] " << std::flush;
    std::vector<GenDim> vgd;

    while (!s.empty() && is)
    {
        istr ic(s);
        string a, b, c;
        std::getline(ic, a, ':'); std::getline(ic, b, ':'); std::getline(ic, c);
        if (!ic) throw "bad range syntax [" + s + "]";

        GenDim gd;
        istr(a) >> gd.min;
        istr(b) >> gd.max;
        istr(c) >> gd.n;

        vgd.push_back(gd);

        is >> s;
    }

    ull sz = 1;
    for (auto x : vgd) sz *= x.n;
    cout << "(" << sz << ") ... " << std::flush;

    add_gen_point(v, vgd, vd());

    cout << "ok\n";
}

void lut_local::add_gen_point(vvd & v, const std::vector<GenDim> & vgd, vd p)
{
    if (p.size() == vgd.size())
    {
        v.push_back(p);
        return;
    }

    GenDim gd = vgd[p.size()];
    if (gd.n < 2) throw "gen points cannot be < 2";
    for (int i = 0; i < gd.n; i++)
    {
        double x = gd.min + i * (gd.max - gd.min) / (gd.n - 1);
        vd q = p;
        q.push_back(x);
        add_gen_point(v, vgd, q);
    }
}

pair<Point, Cube *> Lut::getSubCube(vd point, Cube * parent)
{
    pair<Point, Cube *> r { Point(), nullptr };

    if (!parent) // request for root
    {
        if (!root)
        {
            r.first.coords.resize(cfg.dimX, 0);
            return r;
        }
        return pair<Point, Cube *>(root->point(), root);
    }

    throw "NI Lut::getSubCube\n";

    return r;
}

Cube * Lut::addCube(Point p, Cube * parent)
{
    Cube * c = new Cube(p, parent);

    if (parent)
        parent->addKid(c);
    else
        root = c;

    std::vector<Point> verts = c->listVerts();

    for (auto p : verts) insertLut(p);

    return c;
}

bool Lut::checkPrecX(Cube * cb, const vd & pt, const PtsClass & pc)
{
    throw "NI Lut::checkPrecX";
}

bool Lut::checkPrecY(Cube * cb, const vd & pt, const PtsClass & pc)
{
    throw "NI Lut::checkPrecX";
}

void Lut::insertLut(Point p)
{
    ///throw "NI Lut::insertLut(Point p)";

    // 1 normalize
    // 2 translate
    // 3 exec f
    // 4 insert

    p.normalize();
    lut[p] = f(translate(p));
}

std::vector<Point> Cube::listVerts() const
{
    int vsz = base.coords.size();
    ull ssz = 1 << base.coords.size();

    std::vector<Point> v;
    for (ull i = 0; i < ssz; i++)
    {
        Point x = base;
        auto k = i;
        for (int j = 0; j < vsz; j++)
        {
            x.coords[j] += (k % 2);
            k >>= 1;
        }
        v.push_back(x);
    }

    return v;
}

vd Lut::translate(Point p)
{
    throw "NI Lut::translate";
}

void Point::normalize()
{
    for (;;)
    {
        if ( lev == 0 ) return;
        for ( auto x : coords ) if ( x % 2 ) return;
        for ( auto & x : coords ) x /= 2;
        --lev;
    }
}

bool Point::operator<(const Point & p) const
{
    if ( lev < p.lev ) return true;
    if ( lev > p.lev ) return false;

    auto sz = coords.size();

    if ( sz != p.coords.size() ) throw __LINE__;

    for ( decltype(sz) i = 0; i < sz; i++ )
    {
        if ( coords[i] < p.coords[i] ) return true;
        if ( coords[i] > p.coords[i] ) return false;
    }

    return false;
}


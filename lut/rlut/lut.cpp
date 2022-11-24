#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <iomanip>

#include "lut.h"
#include "f.h"

using std::cout;

namespace lut_local
{
struct GenDim { double min, max; int n; };

string readline(std::istream & in, bool eatempty);
void add_points(string cfg_line, vvd & v, int sz);
void load_from_file(string file, vvd & v, int sz);
void add_gen_point(vvd & v, const std::vector<GenDim> & vgd, vd pt);
} // lut_local

using namespace lut_local;
using istr = std::istringstream;

Lut::Lut(string bname)
{
    cfg.name = bname;

    {
        string file = cfg.name + ".cfg";
        cout << "loading lut config: " << file << '\n';
        std::ifstream in(file);
        if (!in) throw "Cannot open file [" + file + "]";

        // read dimensions
        auto line = readline(in, true);

        if (!(istr(line) >> cfg.dimX >> cfg.dimY))
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
            if (!is) throw file + ": reading X ranges failed [" + line + "]";
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
            if (!in) break;
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
                add_points(line, pc.points, cfg.dimX);
                line = readline(in, false);
            }

            cfg.ptsClasses.push_back(pc);
        }

    }

    {
        string file = cfg.name + ".dat";
        std::ifstream in(file);
        if (!in) { return; }
        cout << "loading lut table: " << file << std::flush;

        for (;;)
        {
            unsigned lev = 0;
            in >> lev;
            if (!in) break;

            Point p; p.lev = lev;

            for (auto i = cfg.dimX * 0; i < cfg.dimX; i++)
            {
                ull x; in >> x; p.coords.push_back(x);
            }

            vd y;
            for (auto i = cfg.dimY * 0; i < cfg.dimY; i++)
            {
                double x; in >> x; y.push_back(x);
            }

            if (!in) throw "File [" + file + "] corrupted";
            tbl[p] = y;
        }

        cout << "\t" << tbl.size() << '\n';
    }

    cout << "reconstructing cubes" << std::flush;
    buildCubes();
    cout << "\t" << stat.totalCubes << '\n';
}

string lut_local::readline(std::istream & in, bool eatempty)
{
    string line;
    std::getline(in, line);
    if (!in) return "";

    while ( line.size() &&
            std::isspace(line[line.size() - 1]) )
        line = line.substr(0, line.size() - 1);

    if (line.empty())
    {
        if (!eatempty) return "";
        return readline(in, eatempty);
    }
    if (line[0] != '#') return line;
    return readline(in, eatempty);
}

void lut_local::load_from_file(string file, vvd & v, int sz)
{
    std::ifstream in(file);
    if (!in) throw "Cannot open [" + file + "]";

    for (string line; std::getline(in, line);)
    {
        if ( line.empty() || line[0] == '#') continue;
        istr is(line);
        vd a;
        for (double x; is >> x;) a.push_back(x);
        int isz = (int)a.size();
        if ( isz < sz) throw "Bad file [" + file + "], line [" + line + "]";
        if ( isz > sz) a.resize(sz);
        v.push_back(a);
    }
}

void lut_local::add_points(string cfg_line, vvd & v, int szx)
{
    istr is(cfg_line);

    string s;
    is >> s;

    if (!is || s.empty()) throw "bad points line [" + cfg_line + "]";

    if (!std::isdigit(s[0]) && s[0] != '-' )
    {
        load_from_file(s, v, szx);
        return;
    }

    cout << "generating points [ " << cfg_line << " ] " << std::flush;
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
    cout << "(" << sz << std::flush;

    add_gen_point(v, vgd, vd());

    cout << ")\n";
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

pair<Point, Cube *> Lut::getSubCube(vd rpoint, Cube * parent) const
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

    auto sz = rpoint.size();
    const Point & base = parent->point();

    if (sz != base.coords.size()) throw ERR;

    // build initial sub-cube
    Point q = base.sub();

    for (auto i = sz * 0; i < sz; i++)
    {
        Point p = base;
        double mn = translate1(p, i);
        ++p.coords[i];
        double mx = translate1(p, i);
        double mid = mn + 0.5 * (mx - mn);

        if (rpoint[i] > mid) ++q.coords[i];
    }

    r.first = q;
    r.second = parent->findKid(q);

    return r;
}

Cube * Lut::addCube(Point p, Cube * parent)
{
    Cube * c = new Cube(p, parent);
    stat.totalCubes++;
    stat.builtCubes++;

    if (parent)
        parent->addKid(c);
    else
        root = c;

    std::vector<Point> verts = c->listVerts();

    for (auto p : verts) insertLut(p);

    return c;
}

bool Lut::checkPrecX(const Cube & cb, const PtsClass & pc) const
{
    const auto & prx = pc.precX;
    auto sz = prx.size();

    const auto & pb = cb.point();

    if ( sz != pb.coords.size() ) throw ERR;

    auto lev = (1ull << pb.lev);
    double cubeSz = 1.0 / lev;

    for ( auto x : prx )
        if ( cubeSz > x )
            return false;

    return true;
}

bool Lut::checkPrecY(const Cube & cb, const PtsClass & pc, const vd & pt) const
{
    vd realf = F(pt);
    vd lnf = linef(cb, pt);

    const auto & prec = pc.precY;
    const auto & rng = cfg.rangeY;

    auto sz = realf.size();

    if ( sz != lnf.size() ) throw "Bad function Y dimension, check F";

    if ( sz != prec.size() ) throw ERR;
    if ( sz != rng.size() ) throw ERR;

    for ( auto i = sz * 0; i < sz; i++ )
    {
        double x = realf[i] - lnf[i];
        if ( x < 0 ) x = -x;

        const auto & g = rng[i];
        double d = g.second - g.first;

        x /= d;
        if ( x > prec[i] )
            return false;
    }

    return true;
}

void Lut::insertLut(Point p)
{
    // 1 normalize point
    // 2 translate to real coords
    // 3 exec f - find value
    // 4 insert to table

    p.normalize();
    tbl[p] = F(translate(p));
}

vd Lut::translate(Point p) const
{
    auto w = (1ull << p.lev);

    const auto & rng = cfg.rangeX;
    auto sz = rng.size();

    if ( sz != p.coords.size() ) throw ERR;

    vd r;

    for ( auto i = sz * 0; i < sz; i++ )
    {
        const double & mn = rng[i].first;
        const double & mx = rng[i].second;
        double z = mn + (mx - mn) * p.coords[i] / w;
        r.push_back(z);
    }

    return r;
}

double Lut::translate1(Point p, size_t i) const
{
    auto w = (1ull << p.lev);

    const auto & rng = cfg.rangeX;
    auto sz = rng.size();

    if (sz != p.coords.size()) throw ERR;

    const double & mn = rng[i].first;
    const double & mx = rng[i].second;
    double z = mn + (mx - mn) * p.coords[i] / w;

    return z;
}

vd Lut::linef(const Cube & cb, const vd & pt) const
{
    // 1 build relative real (rr) point within cube
    //   so its coordinates are 0 to 1 double
    // 2 run recursive function that takes a mix of
    //   discrete coordinates (vu) and real (vd)
    //   using f(x,y,.)=kf(a,y,.)+(1-k)f(a+1,y,.)

    auto sz = pt.size();
    Point b = cb.point();

    if (b.coords.size() != sz) throw ERR;

    vd rr;

    for (auto i = sz * 0; i < sz; i++)
    {
        Point p = b;
        double mn = translate1(p, i);
        ++p.coords[i];
        double mx = translate1(p, i);
        rr.push_back((pt[i] - mn) / (mx - mn));
    }

    return lnfr(b, rr);
}

vd Lut::lnfr(Point p, vd rr) const
{
    if (rr.empty())
    {
        p.normalize();
        auto it = tbl.find(p);
        if (it == tbl.end()) throw "Bad access"; // no value in the table
        return it->second;
    }

    double k = rr[rr.size() - 1];
    rr.pop_back();

    auto index = rr.size();

    vd z = lnfr(p, rr);
    mul(z, 1 - k);
    ++p.coords[index];
    vd y = lnfr(p, rr);
    mul(y, k);
    add(z, y);

    return z;
}

void Lut::buildCubes()
{
    // 1 build root cube
    // 2 call recursive for all kids
    Point p;
    p.coords.resize(cfg.dimX, 0);
    Cube r(p, nullptr);
    if (!isCubeInLut(r)) return;
    root = new Cube(p, nullptr); stat.totalCubes++;
    buildCube(root);
}

void Lut::buildCube(Cube * c)
{
    // 1 generate potential kids
    // 2 check for each if inLut and add to kids
    // 3 call recursive for kids

    Point subbase = c->point().sub();
    Cube subcube(subbase, nullptr);

    auto vxs = subcube.listVerts();

    std::vector<Cube *> vc;

    for (auto p : vxs)
    {
        Cube pk(p, nullptr);
        if (!isCubeInLut(pk)) continue;
        Cube * nc = new Cube(p, c); stat.totalCubes++;
        c->addKid(nc);
        vc.push_back(nc);
    }

    for (auto x : vc) buildCube(x);
}

bool Lut::isCubeInLut(const Cube & c) const
{
    auto vxs = c.listVerts();
    for (auto p : vxs)
    {
        p.normalize();
        if (tbl.find(p) == tbl.end()) return false;
    }
    return true;
}

void Lut::save() const
{
    string file = cfg.name + ".dat";
    std::ofstream of(file);
    if (!of) throw "Cannot open " + file + " for writing";
    for (auto ln : tbl)
    {
        Point p = ln.first;
        of << p.lev;
        for (auto x : p.coords) of << '\t' << x;
        const vd & v = ln.second;
        for (auto x : v) of << '\t' << std::setprecision(OPREC) << x;
        of << '\n';
    }
    cout << "file " << file << " saved\n";
}

const Cube & Lut::cube(const vd & p) const
{
    if (!root) throw "No table defined";
    Cube * cub = root;

    while (1)
    {
        pair<Point, Cube *> pcb = getSubCube(p, cub);
        Cube * cb = pcb.second;
        if (!cb) break;
        cub = cb;
    }

    return *cub;
}



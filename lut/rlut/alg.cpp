#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <functional>
#include <memory>
#include <cstdlib>

#include "lut.h"
#include "alg.h"
#include "f.h"

using std::cout;

void buildLut(Lut & t)
{
    decltype(Config::ptsClasses.size()) cntr = 0, sz = 0;
    for (auto & x : t.cfg.ptsClasses) sz += x.points.size();

    for ( auto & x : t.cfg.ptsClasses )
    {
        for (const auto & pt : x.points)
        {
            if (!(++cntr % 100)) cout << int(100.0 * cntr / sz + 0.5) << "%  \r";

            Cube * cube = nullptr;
            while (1)
            {
                pair<Point, Cube *> pcb = t.getSubCube(pt, cube);
                Cube * cb = pcb.second;
                if (!cb) cb = t.addCube(pcb.first, cube);
                if (t.checkPrecX(*cb, x)) break;
                if (t.checkPrecY(*cb, x, pt)) break;
                cube = cb;
            }
        } // next point
    } // next pt class

    cout << "built: " << t.stat.builtCubes << "\ttotal: " << t.stat.totalCubes << '\n';

    t.save();
}

void runLut(Lut & t, string out)
{
    std::ostream * po = &cout;
    std::unique_ptr<std::ofstream> of;

    if (!out.empty())
    {
        of = std::unique_ptr<std::ofstream>(new std::ofstream(out));
        po = of.get();
    }

    std::ostream & o = *po;

    for (auto & x : t.cfg.ptsClasses)
    {
        for (const auto & p : x.points)
        {
            auto y = t.f(p);
            for (auto x : p) o << std::setprecision(OPREC) << x << '\t';
            for (auto x : y) o << '\t' << std::setprecision(OPREC) << x;
            o << '\n';
        } // next point
    } // next pt class
}

void cmpLut(Lut & t)
{
    decltype(Config::ptsClasses.size()) cntr = 0, sz = 0;
    for (auto & x : t.cfg.ptsClasses) sz += x.points.size();

    if (!sz) throw "No points";

    double resY = 0, resZ = 0;;

    for (auto & x : t.cfg.ptsClasses)
    {
        for (const auto & p : x.points)
        {
            if (!(++cntr % 100)) cout << int(100.0 * cntr / sz + 0.5) << "%  \r";

            auto ly = t.f(p);
            auto fy = F(p);

            double r = 0;
            auto s = ly.size();
            if (s != fy.size()) throw "Bad function Y dimension";

            for ( auto i = s * 0; i < s; i++ )
            {
                double a = ly[i] - fy[i];
                r += a * a;
            }

            const Cube & cb = t.cube(p);
            double d = 1.0 / (1ull << cb.point().lev);
            d *= d;

            resY += r;
            resZ += (r < d ? r : d);
        } // next point
    } // next pt class

    resY = std::sqrt(resY) / sz;
    resZ = std::sqrt(resZ) / sz;

    cout << "Av quad Y-deviation and XY-precision: " << resY << '\t' << resZ << '\n';
}

void drawLutPoint(Lut & t)
{
    //decltype(Config::ptsClasses.size()) cntr = 0, sz = 0;
    //for (auto & x : t.cfg.ptsClasses) sz += x.points.size();

    std::ofstream of(t.cfg.name + ".pnt.dat");

    for (auto & x : t.cfg.ptsClasses)
    {
        for (const auto & pt : x.points)
        {
            //if (!(++cntr % 100)) cout << int(100.0 * cntr / sz + 0.5) << "%  \r";

            for (auto x : pt) of << '\t' << x; of << '\n';
        } // next point
    } // next pt class

    //cout << sz << " points\n";
}

void drawLutEdge(Lut & t, string slev)
{
    // 1 get all cubes
    // 2 for each cube, get all vetrs
    // 3 for each pair of verts build edge if hamming distance 1
    // 4 edge must be unique, e.g. v1 < v2
    // 5 output edges

    int lev = -1;
    if ( !slev.empty() ) lev = std::atoi(slev.c_str());

    std::ofstream of(t.cfg.name + ".edg.dat");
    of << "# level " << lev << '\n';

    Cube * root = t.getRoot();
    if (!root) throw "No cubes";

    std::vector<Cube *> cbs {root};

    std::function<void(Cube *, int)> populate;
    populate = [&cbs, &populate](Cube * c, int lev)
    {
        auto v = c->getKids();
        if ( lev <= 0 ) cbs.insert(cbs.end(), v.begin(), v.end());
        if ( lev ) for (auto x : v) populate(x, lev - 1);
    };

    populate(root, lev);

    auto hamming = [](const vu & v1, const vu & v2) -> int
    {
        auto sz = v1.size();
        if (sz != v2.size()) throw ERR;
        int k = 0;
        for (auto i = sz * 0; i < sz; i++) if (v1[i] != v2[i]) k++;
        return k;
    };

    for (auto c : cbs)
    {
        auto vs = c->listVerts();
        auto sz = vs.size();
        for ( auto i = sz * 0; i < sz; i++ )
            for (auto j = i + 1; j < sz; j++)
            {
                Point p1 = vs[i];
                Point p2 = vs[j];
                if (hamming(p1.coords, p2.coords) > 1) continue;

                auto y1 = t.translate(p1);
                auto y2 = t.translate(p2);

                for (auto x : y1) of << '\t' << x; of << '\n';
                for (auto x : y2) of << '\t' << x; of << "\n\n";
            } // next vx pair
    } // next cube
}


#include <iostream>

#include "lut.h"
#include "alg.h"

using std::cout;

int build(string nm)
{
    Lut t(nm);

    for ( auto & x : t.cfg.ptsClasses )
    {
        for (const auto & pt : x.points)
        {
            cout << "AAA " << pt.size() << "\n";
            Cube * cube = nullptr;
            while (1)
            {
                pair<Point, Cube *> pcb = t.getSubCube(pt, cube);
                Cube * cb = pcb.second;
                if (!cb) cb = t.addCube(pcb.first, cube);
                if (t.checkPrecX(cb, pt, x)) break;
                if (t.checkPrecY(cb, pt, x)) break;
                cube = cb;
            }
        } // next point
    } // next pt class

    return 0;
}

int run(string nm, string pts)
{
    return 0;
}


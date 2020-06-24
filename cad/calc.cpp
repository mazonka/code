#include <iostream>
#include <vector>
#include <string>
#include <cmath>

#include "bad.h"
#include "model.h"
#include "dlibad.h"

using std::cout;

void Model::commit_vals()
{
    int counter = 0;
    auto & myidxs = idxs;
    auto & vals = loss_vals;

    auto fx = [&myidxs, &vals, &counter](Fcoord & x)
    {
        auto id = myidxs[counter++];
        BAD( x.fixed && id != -1 );

        if ( !x.fixed ) x.v = ((signed long)(0.5 + 1000 * vals[id])) / 1000.;
    };

    for ( auto & p : points )
    {
        fx(p.x);
        fx(p.y);
        fx(p.z);
    }

    idxs.clear();
    loss_vals.clear();
}

void Model::cook_idxs()
{
    idxs.clear();
    loss_vals.clear();

    int counter = 0;
    auto & myidxs = idxs;
    auto & vals = loss_vals;

    auto add = [&myidxs, &vals, &counter](Fcoord x)
    {
        if ( x.fixed ) myidxs.push_back(-1);
        else
        {
            myidxs.push_back( counter++ );
            vals.push_back(x.v);
        }
    };

    for ( auto p : points )
    {
        add(p.x);
        add(p.y);
        add(p.z);
    }
}

template <class T> double Model::loss(T t)
{
    ///auto w = 1 + x[0];
    double r = 0;
    for ( auto d : distances )
    {
        auto i1 = d.s.i1;
        auto i2 = d.s.i2;
        Point p1 = points[i1];
        Point p2 = points[i2];

        i1 *= 3; i2 *= 3;

        auto p1x = p1.x.v;
        auto p1y = p1.y.v;
        auto p1z = p1.z.v;
        auto p2x = p2.x.v;
        auto p2y = p2.y.v;
        auto p2z = p2.z.v;

        if ( idxs[i1 + 0] != -1 ) p1x = t[idxs[i1 + 0]];
        if ( idxs[i1 + 1] != -1 ) p1y = t[idxs[i1 + 1]];
        if ( idxs[i1 + 2] != -1 ) p1z = t[idxs[i1 + 2]];
        if ( idxs[i2 + 0] != -1 ) p2x = t[idxs[i2 + 0]];
        if ( idxs[i2 + 1] != -1 ) p2y = t[idxs[i2 + 1]];
        if ( idxs[i2 + 2] != -1 ) p2z = t[idxs[i2 + 2]];

        double x = p1x - p2x;
        double y = p1y - p2y;
        double z = p1z - p2z;
        double xy2 = x * x + y * y + z * z;
        double xyz = std::sqrt(xy2);
        double s = d.d - xyz;
        double s2 = s * s;
        r += s2;
    }
    return r;
}


double g_loss(int n, const double * x, const void * model)
{
    Model * m = (Model *)model;
    BAD( !m );
    return m->loss(x);
}

double Model::calc()
{
    cook_idxs();

    auto r1 = loss(loss_vals);

    Dlibad d(g_loss, (const void *)this);
    loss_vals = d.solve(loss_vals, 1000000);

    auto r = loss(loss_vals);
    commit_vals();
    return r;
}



#include "cube.h"


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

    if ( sz != p.coords.size() ) throw ERR;

    for ( decltype(sz) i = 0; i < sz; i++ )
    {
        if ( coords[i] < p.coords[i] ) return true;
        if ( coords[i] > p.coords[i] ) return false;
    }

    return false;
}

bool Point::operator==(const Point & p) const
{
    if (lev != p.lev) return false;

    auto sz = coords.size();

    if (sz != p.coords.size()) throw ERR;

    for (decltype(sz) i = 0; i < sz; i++)
        if (coords[i] != p.coords[i]) return false;

    return true;
}

Point Point::sub() const
{
    Point q = *this;
    ++q.lev;
    for (auto & x : q.coords) x *= 2;
    return q;
}




void mul(vd & v, double k) { for (auto & x : v) x *= k; }
void add(vd & v, const vd & u)
{
    auto sz = v.size();
    if (sz != u.size()) throw ERR;
    for (auto i = 0 * sz; i < sz; i++) v[i] += u[i];
}




std::vector<Point> Cube::listVerts() const
{
    auto vsz = base.coords.size();
    ull ssz = 1ull << base.coords.size();

    std::vector<Point> v;
    for (ull i = 0; i < ssz; i++)
    {
        Point x = base;
        auto k = i;
        for (auto j = 0 * vsz; j < vsz; j++)
        {
            x.coords[j] += (k % 2);
            k >>= 1;
        }
        v.push_back(x);
    }

    return v;
}

Cube * Cube::findKid(Point p) const
{
    for (auto & c : kids)
        if (p == c->point() ) return c;

    return nullptr;
}

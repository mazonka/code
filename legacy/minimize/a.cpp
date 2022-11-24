#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "alglib/solvers.h"

typedef std::vector<double> vd;

vd solveAB(const vd & A, const vd & B)
{
    vd ret;
    size_t N = B.size();
    alglib::real_1d_array b;
    b.setcontent(N, B.data());

    if ( A.size() != N * N ) throw "Bad matrix";

    alglib::real_2d_array a;
    a.setcontent(N, N, A.data());

    int out;
    alglib::rmatrixsolvefast(a, N, b, out);

    if ( out <= 0 ) return ret; // singular

    for ( size_t i = 0; i < N; i++ ) ret.push_back(b[i]);
    return ret;
}


double func(const vd & x)
{
    if ( x.size() % 2 || x.empty() ) throw "Bad vector";

    const double k = 1;

    auto sq = [](double y) -> double { return y * y; };
    auto len = [&sq](double ax, double ay, double bx, double by) ->
               double { return std::sqrt(sq(ax - bx) + sq(ay - by)); };

    int sz = x.size();

    double r = k * len(0, 0, x[0], x[1]);
    r += k * len(x[sz - 2], x[sz - 1], 1, 0);


    for ( int i = 0; i < sz; i += 2 )
    {
        if (i) r += k * len(x[i - 2], x[i - 1], x[i], x[i + 1]);
        r += x[i + 1];
    }

    return r;
}

int main()
try
{
    if (0)
    {
        vd a = { 1, 2, 2, 3 };
        vd b = { 10, 11 };
        vd r = solveAB(a, b);
        for ( size_t i = 0; i < r.size(); i++ ) std::cout << ' ' << r[i];
    }

    {
        vd x = { 0.5, 0 };
        std::cout << func(x);
        ///x = step(func, x, 1e-3);
    }
}
catch (...)
{
    std::cout << "Error\n";
}

#include "f.h"

using vd = std::vector<double>;

vd f(const vd & x)
{
    double r = 0;

    if ( x[0]*x[0] + x[1]*x[1] < 1 ) r = 1;

    vd v { r };

    return v;
}



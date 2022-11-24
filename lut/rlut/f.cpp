#include <cmath>

#include "f.h"

using vd = std::vector<double>;

vd F(const vd & x)
{
    double r = 0;

    double N = 100;
    double z = std::pow(x[0] + x[1], N);
    if ( x[0]*x[0] + x[1]*x[1] < 0.1 ) r = 1;

    //if ( x[0] + x[1] < 1.5 ) r = 1;

    vd v { z / (1 + z), r };

    return v;
}



// equation a^2 = 2^n - k

#include <cmath>

#include "ccrun.h"
#include "ccrut.h"

using bint = unsigned long long;

inline bint sq(bint x)
{
    return (bint)std::sqrt(1.0 * x);
}

void cmain()
{
    auto sq2 = std::sqrt(2);
    for ( int np = 1; np < 31; np++ )
    {
        bint tn = 1; tn <<= np;
	auto bp = sq2*tn;
	bint b1 = bint(bp);
	double b2 = bp-b1;
	double func = b1*b2;
	///bint a = bint( sq2*tn );
	///bint k = tn*tn*2 - a*a;
	///auto n = np*2+1;

	///bint ar = bint( sq2*tn + 0.5 );
	///long long kr = tn*tn*2 - ar*ar;

        cout << "l=" << np << " func=" << func << '\n';
    }
}

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
    for ( int n = 1; n < 60; n++ )
    {
        bint tn = 1; tn <<= n;
        bint tsq = sq(tn);
        for ( bint k = 1; k <= tsq; k++ )
        {
            bint a2 = tn - k;
            bint a = sq(a2);
            if ( a * a == a2 )
            {
                if ( k <= n )
                {
                    cout << std::dec;
                    cout << "a=" << a << " n=" << n << " k=" << k << " [k<=n]" << '\n';
                    cout << "a2=" << a2 << std::hex << " a2=0x" << a2 << " ah=0x" << a << '\n';
                }
                //cout << "a=" << a << " n=" << n << " k=" << k << " sq=" << tsq << '\n';
            }
        }
    }
}

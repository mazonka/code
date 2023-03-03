// RNS base Y0 Y1 Y2
// with Yi*Yj=d_ij*Yi

#include <algorithm>

#include "ccrun.h"
#include "ccrut.h"

void cmain()
{
    ol::ivec<int> v;
    for_( i, 3 ) for_(j, 5) for_(k, 1)
    {
        v.push_back( (i * 70 + j * 21 + k * 15) % 105 );
    }

    std::sort(v.begin(), v.end());

    for ( auto x : v ) cout << ' ' << x;
	cout<<"\n=["<<v.size()<<"]\n";
}

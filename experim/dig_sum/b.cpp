#include "ccrun.h"
#include "../../hideg/olu.h"
//#include "../../1file/cpp2/coutlog.h"

using num = unsigned long long;

ivec<int> digits(num x)
{
    ivec<int> r;
    while (x)
    {
        r += int(x % 10);
        x /= 10;
    }
    return r;
}

inline num pw(num x, int j)
{
    if (j == 0) return 1;
    if (j == 1) return x;
    num z = 1;
    for (int i = 0; i < j; i++) z *= x;
    return z;
}

int init(num v[])
{
    for ( int j = 0; j <= 10; j++ )
        for ( int x = 0; x < 10; x++ )
            v[j * 10 + x] = pw(x, j);

    return v[10];
}

inline num pwt(int x, int j)
{
    static num v[10 * 11];
    static int dummy = init(v);
    return v[j * 10 + x];
}

void cmain()
{
    num y = 2;
    int j = 6;
    for ( num i = 1; i < 200; i++ )
    {
        auto ds = digits(y);

        num sum = 0; for ( auto x : ds ) sum += pwt(x, j);
        if ( 1 || i == sum ) cout << y << " pw=" << j << " sum=" << sum << '\n';
        y = sum;
    }
}

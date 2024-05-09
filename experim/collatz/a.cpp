#include <cctype>
#include "ccrun.h"

using ull = unsigned long long;

inline ull f2(ull a)
{
    while ( a % 2 == 0 ) a /= 2;
    return a;
}

inline ull f1(ull a)
{
    return (a - 1) / 2;
}

ull x8(ull x)
{
    ull a = 3 * x + 2;
    return f1(f2(a));
}

int len(ull x)
{
    for ( int i = 0; ; i++ )
    {
        if ( x == 0 ) return i;
        x = x8(x);
    }
    never;
}

string bits(ull x)
{
    if ( x < 1 ) return "-";
    string k;
    while (x)
    {
        char c = '-';
        if ( x % 2 ) c = 'x';
        k = c + k;
        x /= 2;
    }
    return k;
}

int H(ull x)
{
    if (x == 0) return 0;
    if (x == 2) return 1;

    if ( x % 2 ) return 1 + H(2 + 3 * (f1(x)));
    x /= 2;
    if ( x % 2 ) return H(f1(x));
    return 1 + H(3 * (x / 2));
}

void lst()
{
    int mx = 0;
    for ( ull i = 0; i < 1000; i++ )
    {
        //if ( (i>>2)*4+2==i ) continue;
        ull k = x8(i);
        //k = x8(k);
        //k = x8(k);
        //k = x8(k);
        int c = len(i);
        //if ( i >= k )
        //if ( c < mx ) continue; mx = c;
        cout << i << '\t' << k << '\t' << c
             //<< '\t' << h
             << '\t' << bits(i) << '\t' << bits(k) << '\n';

        int h = H(i);
        if ( h != c ) never;
    }
}

ull x3to02(ull x)
{
    if ( x % 3 == 1 ) never;
    if ( x % 3 == 0 ) return x / 3 * 2;
    if ( x % 3 == 2 ) return (2 * x - 1) / 3;
    never;
}

ull x02to3(ull x)
{
    if ( x % 2 == 0 ) return (x/2)*3;
    return (3*x+1)/2;
}

void lst02()
{
    int mx = 0;
    for ( ull j = 0; j < 10000; j++ )
    {
        if ( j % 3 == 1 ) continue;
        auto i = x3to02(j);
        //if ( (i>>2)*4+2==i ) continue;
        ull jk = x8(j);
        int c = len(j);
	auto k = x3to02(jk);

	//if( j==k ) continue;
	//if( k==0 ) continue; if( j==(j/k)*k ) continue;
	//auto j2= j-2; if( j2>2 && j2==(j2/k)*k ) continue;

        cout << j << '\t' << i << '\t' << k << '\t' << c
             //<< '\t' << h
             << '\t' << bits(i) << '\t' << bits(k) << '\n';

        //int h = H(i);
        //if( h!=c ) never;
    }
}

ull fromBits(string s)
{
    ull r = 0;
    int sz = s.size();
    for ( int i = 0; i < sz; i++)
    {
        r *= 2;
        if ( s[i] == 'x' || s[i] == '1' ) r += 1;
    }
    return r;
}

void movie()
{
    if ( args.empty() ) return;
    string arg = args[0];

    ull x;
    if ( std::isdigit(arg[0]) )
        x = stoull(arg);
    else
        x = fromBits(arg);

    cout << "Bits: " << bits(x) << '\n';

    while (x)
    {
        cout << x << '\t' << bits(x) << '\n';
        x = x8(x);
    }
}

void cmain()
{
    //lst();
    lst02();
    //movie();
}

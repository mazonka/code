
#include "ccrun.h"
#include "ccrut.h"

using vect = std::vector<int>;

void prn(const vect & v) { for (auto x : v) cout << ' ' << x; }

vect operator*(const vect & a, const vect & b)
{
    auto sz = (int)a.size();
    if ( sz != (int)b.size() ) never;

    vect r(sz, 0);

    int k = 0 * sz / 2;
    //if ( !(sz % 2) ) nevers("need odd size");

    for ( int i = 0; i < sz; i++ )
    {
        for ( int j = 0; j < sz; j++ )
        {
            r[(k + i) % sz] += a[j] * b[ (j + i) % sz ];
            //r[(k + i) % sz] += a[j] * b[ sz-1 - (j + i) % sz ]; // invert
        }
    }
    return r;
}

vect operator+(const vect & a, const vect & b)
{
    auto sz = (int)a.size();
    if ( sz != (int)b.size() ) never;

    vect r(sz, 0);

    for ( int i = 0; i < sz; i++ )
    {
        r[i] = a[i] + b[i];
    }
    return r;
}

void cmain()
{
    vect
    a {1, 2, 3, 4},
    b {2, 5, 7, 11},
    c {6, 1, 13, 5};

    cout << "a: "; prn(a); cout << '\n';
    cout << "b: "; prn(b); cout << '\n';
    cout << "c: "; prn(c); cout << '\n';

    auto ab = a * b;
    auto ba = b * a;
    auto bc = b * c;

    cout << "ab: "; prn(ab); cout << '\n';
    cout << "ba: "; prn(ba); cout << '\n';
    cout << "(ab)c: "; prn(ab * c); cout << '\n';
    cout << "a(bc): "; prn(a * bc); cout << '\n';

    auto b_c = b + c;
    cout << "b+c: "; prn(b_c); cout << '\n';
    cout << "a*(b+c): "; prn(a * b_c); cout << '\n';
    cout << "a*b+a*c: "; prn(a * b + a * c); cout << '\n';

}


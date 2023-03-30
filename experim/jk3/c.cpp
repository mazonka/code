// collatz


#include "ccrun.h"
#include "ccrut.h"

using unt = uint64_t;

string unit = "x";
string zero = " ";

string o(unt i)
{
    std::ostringstream os;
    if ( !(i / 1000) ) os << ' ';
    if ( !(i / 100) ) os << ' ';
    if ( !(i / 10) ) os << ' ';
    os << i;
    return os.str();
}

string bin(unt i)
{
    string r;
    while (i) { r = (i % 2 ? unit : zero) + r; i /= 2; }
    return r;
}

unt next(unt i)
{
    i = 3 * i + 1;
    while ( (i / 2) * 2 == i ) i /= 2;
    return i;
}

string bin(unt i, unt sz)
{
    auto r = bin(i);
    while ( r.size() < sz ) r = zero + r;
    return r;
}

void cmain()
{
    unt start = (((1ull << 1) - 1) << 51 ) + 1;
    //unt start = (((1ull << 1) - 1) << 1 ) + 1;

    for (unt i = start; i < start + 140; i += 2)
    {
        cout << o(i) << ":\n";
        auto j = i;
        unt cntr = 0;
        while (j != 1)
        {
            cout << ' ' << bin(j, 64) << '\n';
            j = next(j);
            cntr++;
        }
        cout << " : " << cntr << '\n';
    }
}


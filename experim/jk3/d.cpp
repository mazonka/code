// collatz
// modification to count only numbers A01
// i e skip forms A011111 since they reduce linearly


#include "ccrun.h"
#include "ccrut.h"

#include <map>

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
    //unt start = (((1ull << 1) - 1) << 51 ) + 1;
    //unt start = (((1ull << 1) - 1) << 1 ) + 1;
    unt  start = 0; //+75529632;


    ol::ivec<unt> v_len, v_i;
    std::map<unt, unt> m;

    bool B = !!true;
    for (unt i = start; i < start + 1000 + 0 * 100000000; i ++ )
    {
        if (B) cout << o(i) << ":\n";
        auto j = 4 * i + 1;
        unt cntr = 0;
	auto j0 = j;
        while (j != 1)
        {
            auto a = (j - 1) / 4;
            if( j>j0 ){ m[a]++; }//cout<<"AAA "<<j <<' '<<j0<<' '<<a<<'\n'; }
            while (1)
            {
                j = next(j);
                if ( (j & 2) == 0 ) break;
            }
            cntr++;
            if (B) cout << ' ' << bin(a, 64) << "| " << a << '\n';
        }
        if (B) cout << " = " << cntr << '\n';

        if ( v_i.empty() || v_len[v_i.size() - 1] < cntr )
        {
            v_i += i;
            v_len += cntr;
        }
    }

    for_i(v_i.size())
    cout << o(v_i[i]) << "\t " << o(v_len[i]) << '\n';

    cout << '\n';
    for (const auto & [k, v] : m)
        cout << o(k) << "\t " << o(v) << '\n';
}


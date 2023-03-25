// this is an interesting number system
// axiom 1=2 for multiplication
// ie work on odd numbers and whenever get even, shift right until odd

#include "ccrun.h"
#include "ccrut.h"


string o(int i)
{
    std::ostringstream os;
    if ( !(i / 1000) ) os << ' ';
    if ( !(i / 100) ) os << ' ';
    if ( !(i / 10) ) os << ' ';
    os << i;
    return os.str();
}


int add(int a, int b)
{
    auto x = 2 * a + 1 + 2 * b + 1;
    while ( x % 2 == 0 ) x /= 2;
    return (x - 1) / 2;
}

int mul(int a, int b)
{
    auto x = (2 * a + 1) * (2 * b + 1);
    while ( x % 2 == 0 ) x /= 2;
    return (x - 1) / 2;
}

void cmain()
{

    auto tbl = [](decltype(add) f)
    {
        int sz = 12;
        cout << "      ";
        for_i(sz) cout << o(i);
        cout << "\n  ---------------------------------\n";

        for_(i, sz)
        {
            cout << o(i) << ": ";
            for_(j, sz)
            {
                cout << o(f(i, j));
            }
            cout << '\n';
        }
    };

    tbl(add);
    tbl(mul);
}


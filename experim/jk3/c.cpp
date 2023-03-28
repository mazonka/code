// collatz


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

string bin(int i)
{
    string r;
    while (i) { r = (i % 2 ? "1" : "0") + r; i /= 2; }
    return r;
}

int next(int i)
{
    i = 3 * i + 1;
    while ( (i / 2) * 2 == i ) i /= 2;
    return i;
}

string bin(int i, int sz)
{
    auto r = bin(i);
    while ( r.size() < sz ) r = "0" + r;
    return r;
}

void cmain()
{
    int start = (1 << 28) + 1;

    for (int i = start; i < start + 10; i += 2)
    {
        cout << o(i) << ":\n";
        auto j = i;
        int cntr = 0;
        while (j != 1)
        {
            cout << '\t' << bin(j, 30) << '\n';
            j = next(j);
            cntr++;
        }
        cout << " : " << cntr << '\n';
    }
}


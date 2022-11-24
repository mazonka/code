#include <iostream>
#include <cstdlib>
#include <iomanip>

using namespace std;

int s(int n, int i)
{
    return n * i;
}

int main(int ac, char * av[])
{
    int sum = 0;
    unsigned long long int sz = (1ULL << 28);
    for ( unsigned long long int i = 0; i < sz; i++ ) sum += s(i, i + 3);
    cout << hex << sz << ' ' << sum << '\n';
}


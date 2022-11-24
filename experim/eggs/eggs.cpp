#include <iostream>
#include <cstdlib>

using namespace std;

int s(int n, int i)
{
    if ( n == 0 || i == 0 ) return 0;
    return 1 + s(n - 1, i - 1) + s(n, i - 1);
}

int main(int ac, char * av[])
{
    if ( ac < 3 )
    {
        cout << "Usage: eggs tries\n";
        return 0;
    }

    int n = atoi(av[1]);
    int i = atoi(av[2]);

    cout << s(n, i) << "\n\n";

    for ( int j = 1; j <= i; j++ )
    {
        for ( int k = 1; k <= n; k++ )
        {
            cout << '\t' << s(k, j);
        }
        cout << '\n';
    }
}


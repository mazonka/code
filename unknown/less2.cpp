bool islt_b(int a, int b)
{
    int x = a - b;
    if ( x == -1 ) return true;
    return (0 != x / (x + 1) );
}

int islt(int a, int b)
{
    int x = a - b;
    return ( x == -1 || x== -2 ) ? 1 : x / (x + 1);
}


#include <iostream>
using std::cout;
int main() // test islt
{
    const int Z = 15;
    for ( int a = -Z; a <= Z; a++ )
    {
        for ( int b = -Z; b <= Z; b++ )
        {
            if ( islt(a, b) != int(a < b) )
                std::cout << "ERROR\n";
        }
    }


    for ( int a = -Z; a <= Z; a++ )
    {
        for ( int b = -Z; b <= Z; b++ )
        {
            cout << ' ' << islt(a, b);
        }
        cout << '\n';
    }

///	std::cout<<sizeof(int)<<'\n';
}

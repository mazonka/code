#include <iostream>
#include <cstdlib>
#include <sstream>

using namespace std;

string ort(string a, string b)
{
    int k = 0;
    for ( size_t i = 0; i < a.size(); i++ )
        if ( a[i] == b[i] ) ++k;
        else --k;

    ostringstream os;
    os << k;

    string r = os.str();
    while ( r.size() < 3 ) r += ' ';

    return r;
}

int main(int ac, char * av[])
{
    int nb = 8;

    string s[] =
    {
          "00000111"
        , "10010001"
        , "10001010"
        , "01010010"
        , "01001001"
        , "00111000"

    };

    int sz = sizeof(s) / sizeof(s[0]);

    for ( int i = 1; i <= nb; i++ )
    {
        for ( int j = i + 1; j <= nb; j++ )
        {
            string x;

            cout << i << j << '\t';
            for ( int k = 0; k < sz; k++ )
                if ( s[k][i - 1] == '1' || s[k][j - 1] == '1' ) cout << (k + 1);

            cout << '\n';
        }
    }


    // ortog
    for ( int i = 0; i < sz - 1; i++ )
    {
        for ( int j = i + 1; j < sz; j++ )
        {
            cout << ' ' << i << ':' << j << '=' << ort(s[i], s[j]);
        }
        cout << '\n';
    }
}


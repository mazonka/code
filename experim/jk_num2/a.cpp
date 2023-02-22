#include <iostream>
#include <string>

using std::string;
using std::cout;


struct kj
{
    int k, j;
    static int m;
    kj(int a = 0, int b = 0): k(b), j(a) {}
    void next();
    string str(bool full = true) const;
    kj operator*(kj a) const;
};

int kj::m = 32;

void kj::next()
{
    if ( ++k < m ) return;
    k = 0;
    if ( ++j < m ) return;
    j = 0;
}

string kj::str(bool full) const
{
    if (full)
    {
        string r;
        r += "(";
        r += std::to_string(j);
        r += ',';
        r += std::to_string(k);
        r += ')';
        return r;
    }

    auto z = [](int x) -> char
    {
        if (x < 10) return char(x + '0'); return x - 10 + 'a';
    };

    string r;
    r += z(j);
    r += z(k);
    return r;
}

kj kj::operator*(kj a) const
{
    kj z;
    z.j = k * a.k % m;
    z.k = ( j * a.j + j * a.k + k * a.j ) % m;
    return z;
}

int main()
{
    kj x, y;

    int m2 = kj::m; m2 *= m2;

    cout << "   | ";
    for ( int j = 0; j < m2; j++ )
    {
        cout << ' ' << x.str(0);
        x.next();
    }
    cout << "\n-------------------------\n";

    for ( int i = 0; i < m2; i++ )
    {
        cout << y.str(0) << " | ";
        for ( int j = 0; j < m2; j++ )
        {
            cout << ' ' << (x * y).str(0);
            x.next();
        }
        cout << '\n';
        y.next();
    }
}


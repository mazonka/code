/*
    jk numbers (j,k) (nand numbers)
    j^2 = k
    k^2 = j
    j*k=k*j=k

    Exclude (0,0)

    For all possible x and y, Lets call:
        f1(x)=x^3=x^2*x = n of class K
        f0(x)=x^3^2=x^3*x^3 = n of class J
        n of class Mix, if n=f0(x)=f1(y)
        n of class 0, if no exist x such that n=x*y

    if x in K and y in K: n is Mix, if x*y=n in K
    if x in K and y in J: n is Mix, if x*y=n in J
    if x in J and y in J: n is Mix, if x*y=n in J

    For all possible x, lets call:
        x stable, if x and x^2 are of opposite classes
        x unstable, if x and x^2 are of the same class

    Purpose: find if any modular combinations produce Mix classes
    Also which modular combinations produce no Mix classes - clean

    Maybe Mix class is clean, i.e. not mixing with J/K
    Maybe there are more classes not mixing with J/K
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;
using std::string;

using num = int;

num mJ = 4, mK = 4;

struct JK { num j, k; };
inline JK operator*(JK a, JK b) { return JK { (a.k * b.k) % mJ, (a.j * b.j + a.j * b.k + a.k * b.j) % mK }; }

struct Table
{
    std::vector<int> v;
    Table(): v(mJ * mK, 0) {}
    int & operator[](JK x) { return v[mK * x.j + x.k]; }
    void save(string);
};

const int K_class = 1;
const int J_class = 2;
const int M_class = K_class | J_class;

#define FORjk(j,k) for ( num j = 0; j < mJ; j++ ) for ( num k = 0; k < mK; k++ )
#define FORJK FORjk(j,k)

int main(int ac, char * av[])
{
    if ( ac > 1 ) mK = mJ = stoi(av[1]);
    if ( ac > 2 ) mK = stoi(av[2]);

    Table t;

    FORJK
    {
        auto x = JK {j, k};
        auto y = x * x * x;
        t[y] |= K_class;
        //t[y * y] |= J_class;
    }

    for ( int i = 0; i < 100; i++ )
    {
        FORJK
        {
            auto x = JK {j, k};
            if ( t[x] == K_class ) t[x * x] |= J_class;
            if ( t[x] == J_class ) t[x * x] |= K_class;
        }

        FORjk(j1, k1) FORjk(j2, k2)
        {
            auto x = JK {j1, k1};
            auto y = JK {j2, k2};
            if ( t[x] == K_class && t[y] == J_class ) t[x * y] |= K_class;
        }
    }

    t.save("a");
}

void Table::save(string f)
{
    for ( int c = 0; c < 4; c++)
    {
        ofstream of(f + to_string(c) + ".dat");
        for ( num j = 0; j < mJ; j++ )
        {
            for ( num k = 0; k < mK; k++ )
            {
                if ( (*this)[ {j, k}] != c ) continue;
                of << k << ' ' << j << '\n';
            }
        }
    }
}

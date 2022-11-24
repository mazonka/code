// same but each salve knows the number of poisons
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

using ull = unsigned long long;

ull P = 2;
ull S = 5;
ull B = 12;
ull N; // 1ull<<S

ull bits(ull x);
string bin(ull x, ull sz, bool inv = true);
int process(ull code);
string death(ull pois);
ull spread(ull code, ull shortpois);

ull debin(string s)
{
    ull r = 0;

    for ( size_t i = 0; i < s.size(); i++ )
        if ( s[s.size() - 1 - i] == '1' ) r |= (1ull << i);

    return r;
}

bool perm_next(ull & z, ull b, ull n)
{
    if ( z == 0 )
    {
        for ( ull i = 0; i < b; i++ ) { z <<= 1; z |= 1; }
        return true;
    }

    string p = bin(z, n, false);
    bool r = next_permutation(begin(p), end(p));
    z = debin(p);
    return r;
}

int x()
{
    for (ull z = 0; perm_next(z, B, N); )
    {
        cout << "x " << z << ' ' << bin(z, N) << '\n';
    }
    return 0;
}

int main(int ac, char * av[])
{
    N = (1ull << S);
    if ( B < P ) B = P;
    if ( B > N ) B = N;

    cout << "Starting P=" << P << " S=" << S << " B=" << B << " N=" << N << '\n';

    ///return x();

    ull N2 = (1ull << N);
    double dN2 = 1; for ( ull i = 0; i < N; i++ ) dN2 *= 2.0;
    //for( ull i=N2-1; i>0; i-- )
    //for( ull i=0; i<N2; i++ )
    ull cntr = 0;
    for ( ull i = 0; perm_next(i, B, N); )
    {
        ///cout<<"N2="<<N2<<" i="<<i<<" bits="<<bits(i)<<'\n';
        if ( bits(i) != B ) continue;
        int res = process(i);
        if ( !(++cntr % 50000) ) cout << (1.0 * i / dN2) << ' ' << i << '\r';
        if ( res ) cout << bin(i, N) << ' ' << bits(i) << ' ' << i << " result: " << res << '\n';
    }

    //process(26);
    //death(18);
}


string bin(ull x, ull sz, bool inv)
{
    string r;
    for ( ull i = 0; i < sz; i++ )
    {
        char c = (x % 2 ? '1' : '0');
        if ( inv ) r += c;
        else r = c + r;
        x >>= 1;
    }
    return r;
}

ull bits(ull x)
{
    ull r = 0;
    ull sz = sizeof(x);
    sz <<= 3;
    for ( ull i = 0; i < sz; i++ )
    {
        if ( x % 2 ) ++r;
        x >>= 1;
        ///cout<<bin(x,16)<<' '<<r<<'\n';
    }
    return r;
}

int process(ull colcode)
{
    //cout<<"process "<<bin(colcode,N)<<'\n';

    set<string> uniq;

    ull B2 = (1ull << B);
    //for( ull i=0; i<B2; i++ )
    for ( ull i = 0; perm_next(i, P, B); )
    {
        ///cout<<"proc1 B2="<<B2<<" i="<<i<<" bits="<<bits(i)<<'\n';
        if ( bits(i) != P ) continue;
        ull j = spread(colcode, i);
        string d = death(j);
        ///cout<<"proc2 "<<bin(i,B)<<' '<<bin(j,N)<<' '<<bin(d,S)<<' '<<i<<'\n';

        if ( uniq.find(d) != uniq.end() ) return 0;
        uniq.insert(d);
        ///if( v[int(d)] ) return 0;
        ///v[int(d)] = true;
    }

    return 1;
}

ull spread(ull code, ull sp)
{
    ull r = 0, c = 0;
    while ( sp )
    {
        if ( code % 2 )
        {
            if ( sp % 2) r |= (1ull << c);
            sp >>= 1;
        }
        code >>= 1;
        ++c;
    }

    return r;
}


string d0()
{
    string r;
    for ( ull i = 0; i < S; i++ ) r += '0';
    return r;
}

// this function returns death pattern
// pois is distribution of poison in colums
string death(ull pois)
{
    static string s0 = d0();
    string r = s0;

    ///cout<<"death pois="<<bin(pois,N)<<'\n';

    ///ull r = 0;

    for ( ull i = 0; i < N; i++ )
    {
        ull c = (1ull << i) & pois;
        if ( !c ) continue;
        ///cout<<"A "<<bin(i,S)<<'\n';
        //r |= i;
        for ( ull j = 0; j < S; j++ )
        {
            ull e = (1ull << j) & i;
            //if( e ) r[j] = '1'; // original
            if ( e ) r[j] += 1; // twist
        }
    }

    ///cout<<"A "<<bin(r,S)<<'\n';
    return r;
}


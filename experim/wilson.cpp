#include "ccrun.h"

int fac(int x, int m)
{
    int s = 1;
    for ( int i = 1; i <= x; i++ )
    {
        s *= i;
        s %= m;
    }
    return s;	
}

void cmain()
{
    int p = 67;

    int s = 1;
    for ( int i = 0; i <= p; i++ )
    {
        //s *= i;
        //s %= p;
	auto f = fac(i,p);
	int j = p-i-1;
	//auto cnj = fac(i-1,p)*fac(j,p);
	auto cnj = f*fac(j,p);
	cnj %= p;
        cout << i << '\t' << f << '\t' << cnj << '\n';
    }
}

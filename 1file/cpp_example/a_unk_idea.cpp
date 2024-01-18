#include "ccrun.h"

int m = 17;

int pw(int x, int n)
{
	int r=x;
	while( --n ) r *= x;
	return r;
}

int f(int i)
{
	int x = (( pw(i,4) - 1 + m) % m);

	//if( x==m-1 ) x=-1;
	return (x+i)%m;
}

void cmain()
{
    for ( int i=0; i < m; i++ )
    {
        cout << i << ":\t" << f(i) << '\t' << f(f(i))<< '\n';
    }
}

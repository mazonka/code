#include "ccrun.h"

void cmain()
{

	for( int i=0; i<10000; i++ )
	{
		int j = 2*i+1;
		int a = 3*j+1;
		while( a%2 == 0 ) a/=2;
		int k = (a-1)/2;
		if( i>=k ) cout<<i<<'\t'<<k<<'\n';
	}
}

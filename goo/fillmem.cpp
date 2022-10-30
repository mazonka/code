#include <iostream>
#include <cstdlib>
#include <string>

int main(int ac, const char *av[])
{
	if( ac < 2 ){ std::cout<<"Use: sizeMb or 0\n"; return 0; }

	int sz = std::atoi(av[1]);

	if( sz > 0 )
	{

	for( int i=0; i<sz; i++ )
	{
		for( int j=0; j<1024; j++ )
			void * p = new char[1024];

		std::cout<<i+1<<'\r';
	}

	}
	else 
	{
		//while(1)
		{
			unsigned long long x = 0; --x; x>>=1; ++x;
			std::cout<<std::hex<<x<<'\n';
		}
	}
}

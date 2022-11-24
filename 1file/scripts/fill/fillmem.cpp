#include <iostream>
#include <cstdlib>

int main(int ac, const char *av[])
{
	if( ac < 2 ){ std::cout<<"Use: sizeMb\n"; return 0; }

	for( int i=0; i<std::atoi(av[1]); i++ )
	{
		for( int j=0; j<1024; j++ )
			void * p = new char[1024];

		std::cout<<i+1<<'\r';
	}
}

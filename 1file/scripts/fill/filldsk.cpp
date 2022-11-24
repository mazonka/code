#include <iostream>
#include <fstream>
#include <cstdlib>

int main(int ac, const char *av[])
{
	if( ac < 3 ){ std::cout<<"Use: file sizeMb\n"; return 0; }

	std::ofstream of(av[1],std::ios::binary);
	
	char b[1024];
	for( int i=0; i<std::atoi(av[2]); i++ )
	{
		for( int j=0; j<1024; j++ )
			of.write(b,1024);

		std::cout<<i+1<<'\r';
	}
}

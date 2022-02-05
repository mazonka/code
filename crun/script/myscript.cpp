#include <iostream>

int main(int ac, const char *av[])
{
	std::cout<<"hello from myscript\n";
	for( int i=0; i<ac; i++ ) std::cout<<"["<<av[i]<<"]\n";
}

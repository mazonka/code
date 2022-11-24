#include <iostream>

#include "gn.h"

int main()
{
	std::cout<<NMAX<<"\n\n";

	std::cout<<"Think of a number. I will try to geuss it by your"
		" description. You can say 'ask me'. Type 'bye' to quit.\n";

	while(1)
	{
		std::cout<<"\n> ";
		string line;
		std::cin>>line;

		if( line == "bye" ) break;
	}

	std::cout<<"Bye-bye\n";
}


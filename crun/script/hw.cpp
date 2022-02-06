#include <iostream>

#include "crun.h"

void cmain()
{
	std::cout<<"hello from 'hw'\n";
	cout<<"#args="<<args.size()<<'\n';
	for( auto a : args ) cout<<"["<<a<<"]\n";
}

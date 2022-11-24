#include <iostream>
#include <vector>
#include <string>
#include <exception>

using std::string;
using std::vector;
using std::cout;

void main_args(vector<string> args);
int main(int ac, const char * av[])
try
{
	vector<string> args;
	for( int i=0; i<ac; i++ ) args.push_back(av[i]);
	main_args(args);
}
catch (int * e){ cout << "Error: " << e << "\n"; }
catch (const char * e){ cout << "Error: " << e << "\n"; }
catch (string e){ cout << "Error: " << e << "\n"; }
catch (std::exception e){ cout << "Error: " << e.what() << "\n"; }
catch (...){ cout << "Error: unknown exception\n"; }

void START(){}


#include "body.cpp"


void main_args(vector<string> args){ START; }



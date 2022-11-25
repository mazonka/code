#include <iostream>
#include <cstdlib>
#include <string>

using namespace std;

int main(int ac, const char *av[])
{
	///cout<<ac<<'\n';
	string cmd = "C:\\apg\\app\\bin\\gf.EXE g ";
	///cout<<"AAA1 "<<cmd<<'\n';
	if( ac < 2 ) return 0;
	///cout<<"AAA2 "<<cmd<<'\n';
	cmd += av[1];
	///cout<<"AAA3 "<<cmd<<'\n';
	//std::system("C:\\apg\\app\\bin\\gf.EXE info");
	std::system(cmd.c_str());
}

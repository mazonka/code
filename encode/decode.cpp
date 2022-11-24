
#include <iostream>
#include <cstdlib>
#include <string>

using namespace std;

int s1=1, s2=1, L=10000, M=1000003;

int gen()
{
	s2 += s1; s2 += s2; s1 += s1;
	if( s1>=M ) s1%=M;
	if( s2>=M ) s2%=M;
	return s2;
}

void swap(string &s, int i, int j)
{
	char x = s[i];
	s[i] = s[j];
	s[j] = x;
}

void process(string &s)
{
	int sz = s.length();
	if( sz < 2 ) return;

	for( int i=0; i<L; i++ )
	{
		int j1 = gen();
		int j2 = gen();

		swap(s,j1%sz,j2%sz);
	}
}


int main(int ac, char *av[])
{
	if( ac<2 )
	{
		cerr<<"Usage: decode s1 s2 L M (optional)\n";
	}


	if( ac>1 ) s1 = atoi(av[1]);
	if( ac>2 ) s2 = atoi(av[2]);
	if( ac>3 ) L = atoi(av[3]);
	if( ac>4 ) M = atoi(av[4]);

	cerr<<"Using S1,S2,L,M : "<<s1<<' '<<s2<<' '<<L<<' '<<M<<'\n';

	string s;

	while(1)
	{
		char a;
		cin.get(a);
		if( !cin ) break;
		s+=a;
	}

	process(s);

	cout<<s;

}

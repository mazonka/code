// more efficient way is to mix an array of integers
// and then reconstruct by indices
// but ... its easier just to run the sequence to the end

#include <iostream>
#include <cstdlib>
#include <string>
#include <cctype>

using namespace std;

int s1=1, s2=1, L=10000, M=1000003;

int gen()
{
	s2 += s1; s2 += s2; s1 += s1;
	if( s1>=M ) s1%=M;
	if( s2>=M ) s2%=M;
	return s2;
}

int gen2()
{

	int s10=s1, s20=s2;
	static int b=0;
	int x;
	int run = 2*L-b++;	

        for ( int i=0; i<run; i++ ) x=gen(); 

	s1=s10; s2=s20;
	return x;
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
		int j1 = gen2();
		int j2 = gen2();

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

	int ln=1;

	while(1)
	{
		char a;
		cin.get(a);
		if( !cin ) break;
		s+=a;

		if (a=='\n') ln++;
		if ( !isprint(a) && !isspace(a) ) 
			cerr<<"Warning: line "<<ln<<" bad character "
			<<int((unsigned char)a)
			<<((128<(unsigned char)a)?a:' ')<<'\n';
		else if ( a<1 || a>126 ) 
			cerr<<"Warning: line "<<ln<<" bad character "
			<<int((unsigned char)a)<<' '<<a<<'\n';
	}

	if( s.size() && ( isspace(s[0]) || isspace(s[s.size()-1]) ) )
		cerr<<"Warning: first or last char is space or new line; "
		"may not work properly in some web browsers\n";

	process(s);

	cout<<s;

}

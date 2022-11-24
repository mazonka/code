// Toga Enhanced emulator
// Oleg Mazonka 2010

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <vector>

using std::string;
using std::vector;
using std::cout;
using std::cin;
using std::istream;
using std::ostream;
using std::pair;

namespace g{
int base=32, base_k, base_w;
bool intio = false;
bool conin = false;
bool dbg = false;
}

class Memory
{
	vector<int> m;
public:
	void add(int x){ m.push_back(x); }
	int operator[](int ip) const;
	bool invert(int a);
	bool empty() const { return m.empty(); }
	bool getbit(int a) const;
};



void usage();
void setbase();
Memory readbcc(istream &is);
void execute(Memory &m);
string tos(int x){ std::ostringstream os; os<<x; return os.str(); }
inline string tos(size_t x){ return tos(int(x)); }

int main(int ac, char *av[])
{
  try{
	if( ac < 2 ) usage();

	int input_i = 1;

	for(;;)
	{
		string s;
		if( input_i < ac ) s = string(av[input_i]);
		if( s.empty() || s[0]!='-' ) break;
		else if( s=="-i" ) g::intio=true;
		else if( s=="-c" ) g::conin=true;
		else if( s=="-d" ) g::dbg=true;
		else usage();
		if( ++input_i >= ac && !g::conin ) usage();
	}

	setbase();

	string input_name("te.run");
	if( input_i < ac ) input_name = av[input_i];

	std::istream * in = &std::cin;
	std::ifstream fin;

	if( !g::conin )
	{
		fin.open(input_name.c_str());

		if( !fin )
		{
			cout<<"cannot open input file '"<<input_name<<"'\n";
			return 1;
		}
		in = &fin;
	}

	Memory mem = readbcc(*in);

	if( !mem.empty() ) execute(mem);

  }
  catch(string s)
  {
	cout<<"Error: "<<s<<'\n';
	return 3;
  }
  catch(...)
  {
	cout<<"Unknown error\n";
	return 4;
  }


}

void usage()
{
	cout<<"Useage: terun [-i] [-c] [-d] input.run\n";
	cout<<"-i integer (but byte limited) input/output, default char\n";
	cout<<"-c input from stdin\n";
	cout<<"-d debug (Hint: use -t in assembler)\n";
	exit(0);
}

void setbase()
{
	g::base_w = g::base-1;
	if( g::base == 8 )	g::base_k = 3;
	else if( g::base == 16 ) g::base_k = 4;
	else if( g::base == 32 ) g::base_k = 5;
	else 
		usage();
}

Memory readbcc(istream &is)
{
	Memory m;

	for(;;)
	{
		int i=0;
		is>>i;
		if( !is )
			break;
		m.add(i);
	}

	return m;
}

bool toglebit(Memory &m, int a);

void execute(Memory &m)
{
	int ip=0;

	for(;;)
	{

		if(g::dbg) cout<<ip<<": "<<m[ip]<<" "<<m[ip+g::base]<<" ("
			<<(m.getbit(m[ip])?"1":"0")<<")\n";

		int a = m[ip]; ip+=g::base;

		if ( toglebit(m,a) ) ip = m[ip];
		else ip+=g::base;

		if( ip < 0 )
			return;
	}
}

bool output(bool bit);
bool input();
bool toglebit(Memory &m, int a)
{
	if( a==-1 ) return output(1);
	if( a==-2 ) return output(0);
	if( a==-3 ) return input();
	return m.invert(a);
}

int Memory::operator[](int ip) const
{
	if( ip % g::base )
		throw string()+"jump to non-aligned address not supported ip="+tos(ip);

	int i = ip / g::base;

	if( i>= (int)m.size() || i<0 )
		throw string()+"bad ip; ip="+tos(ip)+" mem_size="+tos(m.size());

	return m[i];
}

bool Memory::invert(int b)
{
	int i = b / g::base;
	int j = ( 1 << ( b % g::base ) );

	if( i==(int)m.size() ) m.push_back(0);

	if( i>= (int)m.size() || i<0 )
		throw string()+"bad access; B="+tos(b)+" mem_size="+tos(m.size());

	if( m[i] & j )
		m[i] &= ~j;
	else
		m[i] |= j;

	return ( m[i] & j );
}

bool Memory::getbit(int a) const
{
	if( a<0 )
		return false;

	int i = a / g::base;
	int j = a % g::base;

	if( i>= (int)m.size() || i<0 )
		throw string()+"bad access; A="+tos(a)+" mem_size="+tos(m.size());

	int mem = m[i];

	if( mem & (1<<j) ) return true;

	return false;
}


void input(char &c);
bool input()
{
	bool bit;
	static vector<bool> buf;

	if( buf.empty() )
	{
		char c=EOF;
		input(c);
		if( c==EOF ) return false;

		for( int i=0; i<8; i++ )
	            buf.push_back( (c & ( 1 << (7-i) ))?true:false );
	}

	bit = buf.back();
	buf.pop_back();
	return bit;
}

void output(char c);
bool output(bool bit)
{
	const bool ret = true;

	static vector<bool> buf;

	if( buf.size() < 8 )
		buf.push_back(bit);

	if( buf.size() < 8 )
		return ret;


	char c=0;
	
	for( int i=0; i<8; i++ )
	{
		int j = ( 1 << i );
		if( buf[i] )
		 	c = char(c|j);
		else
			c = char(c&~j);
	}

	output(c);
	buf.clear();

	return ret;
}

void input(char &c)
{
	if( g::conin )
	throw string()+"No console input when code read from stdin (-c)";

	if( g::intio )
	{
		string s;
		cin>>s;
		if( !cin ) return;
		int x = atoi(s.c_str());
		c = (char)x;
		return;
	}

	cin.get(c);
}

void output(char c)
{
	if( g::intio )
	{
		int x = (unsigned)(unsigned char)c;
		cout<<x<<' '<<std::flush;
		return;
	}

	cout<<c<<std::flush;
}

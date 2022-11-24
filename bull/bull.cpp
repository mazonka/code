#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>

using std::vector;
using std::cout;
using std::cin;
using std::string;

const int SZ=4;
int nmax;
vector<int> gn;
double timeout=100000;
int hint;

bool valid(int x);
int guess();
void dump(const vector<int> n);
string mks(int x);
void getbc(int n1, int n2, int &b, int &c);
vector<int> process(const vector<int> n, int g, int b, int c);
void init(vector<int> &n);


vector<string> gmks;
inline const char * mks_fast(int x){ return gmks[x].c_str(); }
void initmks();

int main(int ac, char *av[])
{
	if( ac>2 ) timeout = std::atof(av[2]);
	if( ac>1 ) hint = std::atoi(av[1]);

	nmax = 1; for( int i=0; i<SZ; i++ ) nmax*=10;

	init(gn);
	initmks();

	cout<<"Usage: bull [help number] [timeout in ms] (e.g. bull 1234 1000)\n\n";

	while(1)
	{
		cout<<"Have you thought a number? (type 'y' and enter) ";
		string a;
		cin>>a;
		if( a=="y" || a=="Y" ) break;
	}

	char *t[10]={ "first", "second", "third", "fourth", "fifth", 
		"sixth", "seventh", "eighth", "ninth", "tenth" };

	int ctr=0;
	int rnd = std::clock();
	int g = gn[ rnd%gn.size() ];
	//*g = guess();
	while(1)
	{

		cout<<"My ";
		if( ++ctr<11 ) cout<<t[ctr-1]; else cout<<ctr<<"th";
		cout<<" guess is "<<mks(g);

		if( hint )
		{
			int xb, xc;
			getbc(g,hint,xb,xc);
			cout<<"  [ for "<<hint<<" bulls="<<xb<<" cows="<<xc<<" ]";
		}

		cout<<'\n';

		cout<<"How many guessed in place (bulls): ";
		int b=0;
		cin>>b;
		cout<<"How many guessed not in place (cows): ";
		int c=0;
		cin>>c;

		gn = process(gn,g,b,c);

		//*dump(gn);

		if( gn.empty() )
		{
			cout<<"Impossible - you made an error\n";
			break;
		}

		if( gn.size()==1 )
		{
			cout<<"Your number is "<<gn[0]<<'\n';
			break;
		}

		g = guess();
	}
}

void init(vector<int> &n)
{
	for( int i=0; i<nmax; i++ )
	{
		if( valid(i) ) n.push_back(i);
	}
}

string mks(int x)
{
	std::ostringstream os;
	os<<x;
	string s = os.str();
	while( s.size() < SZ ) s = "0"+s;
	return s;
}

bool valid(int x)
{
	string s = mks(x);

	for( int i=0; i<SZ; i++ )
	for( int j=0; j<SZ; j++ )
	{
		if( i==j ) continue;
		if( s[i]==s[j] ) return false;
	}

	return true;
}

void dump(const vector<int> n)
{
	cout<<"[";
	for( int i=0; i<n.size(); i++ )
	cout<<' '<<mks(n[i]);
	cout<<" ]\n";
}

void getbc(int n1, int n2, int &b, int &c)
{
	const char * s1 = mks_fast(n1);
	const char * s2 = mks_fast(n2);

	b=0;
	c=0;

	for( int i=0; i<SZ; i++ )
	for( int j=0; j<SZ; j++ )
	{
		if( s1[i]==s2[j] )
		{
			if( i==j ) ++b;
			else ++c;
		}
	}
}

inline void getb(int n1, const char * s2, int &b)
{
	const char * s1 = mks_fast(n1);

	b=0;

	for( int i=0; i<SZ; i++ )
	{
		if( s1[i]==s2[i] ) ++b;
	}
}

vector<int> process(const vector<int> n, int g, int b, int c)
{
	vector<int> r; r.reserve(n.size());

	vector<int> rm; rm.reserve(n.size());

	const char * s2 = mks_fast(g);
	for( int i=0; i<n.size(); i++ )
	{
		int mb, mc;
		getb(n[i],s2,mb);
		if( mb!=b ){ rm.push_back(i); continue; }
		
		getbc(n[i],g,mb,mc);
		if( mb!=b || mc!=c ) rm.push_back(i);
	}

	{
		for( int k=0,i=0; i<n.size(); i++ )
		{
			if( k<rm.size() && rm[k]==i ) k++;
			else r.push_back(n[i]);
		}
	}

	return r;
}

int processq(const vector<int> n, int g, int b, int c)
{
	int r=n.size();

	const char * s2 = mks_fast(g);
	for( int i=0; i<n.size(); i++ )
	{
		int mb, mc;
		getb(n[i],s2,mb);
		if( mb!=b ){ --r; continue; }
		
		getbc(n[i],g,mb,mc);
		if( mb!=b || mc!=c ) --r;
	}

	return r;
}

int guess()
{
	int nmin = gn.size(), inmin=-1;

	double tm = std::clock()+timeout;

	for( int i=0; i<gn.size(); i++ )
	{
		int iq;
		int nnmax=0;
		for( int b=0; b<=SZ; b++ )
		for( int c=0; c<=SZ-b; c++ )
		{
			if( c==1 && b+c==SZ ) continue;
			iq = processq(gn,gn[i],b,c);
			if( iq==0 ) continue;
			if( nnmax<iq ) nnmax = iq;
		}

		if( nnmax < nmin )
		{
			nmin = nnmax;
			inmin = i;
		}

		if( inmin>=0 && double(std::clock()) > tm ) break;
	}

	cout<<"Guess took: "<<double(std::clock())-tm+timeout<<'\n';

	return gn[inmin];
}

void initmks()
{
	for( int i=0; i<nmax; i++ ) gmks.push_back(mks(i));
}


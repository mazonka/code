#include <iostream>
#include <sstream>
#include <string>

using namespace std;

template<typename T>
inline string tos(T x) { std::ostringstream o; o << x; return o.str(); }

int sumdig(string s)
{
	if( s.empty() ) return 0;
	//cout<<s<<':';
	int sum = 0;
	for( auto i=s.size(); i>0 ; --i )
	{
		//cout<<" "<<i<<'.'<<s[i-1];
		sum += char(s[i-1])-'0';
	}
	//cout<<'\n';

	return sum;
}

int main()
{
	using ll = long long;

	ll SZ = 1000000LL*100;
	ll sum = 0;

	string * p = new string[SZ];
	cout<<"checkpoint 1\n";

	for( ll i=0; i<SZ; i++ ) p[i] = to_string(i);	
	cout<<"checkpoint 2\n";

	for( ll i=0; i<SZ; i++ ) sum += sumdig(p[i]);
	cout<<"hi sum="<<sum<<"\n";
}

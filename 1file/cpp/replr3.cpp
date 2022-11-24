// replaces substrings in file
// usage in-file out-file file1 file2 file3 ...
// e.g. replr vx1.vmdk vx2.vmdk file - file.in -> file.out

#include <iostream>
#include <fstream>
#include <string>
#include <map>

using std::string;
using std::cout;

string file2str(string f)
{
	string r;

	std::ifstream in(f.c_str(), std::ios::binary);
    if( !in ) throw "cannot open "+f;
	while(1)
	{
		char a;
		in.get(a);
		if(!in) break;
		r += a;
	}

	return r;
}

int replaceAll(string & s, const string & r, const string & to)
{
    if( r==to ) return 0;

    if( to.find(r)!=string::npos )
    {
        const int SZ = 30;
        string r1 = r; if( r.size() > SZ ) r1=r.substr(0,SZ);
        string t1 = to; if( to.size() > SZ ) t1=to.substr(0,SZ);
        throw "Recursive substitution ["+r1+"]->["+t1+"]";
    }

    int ret=0;
    while (true)
    {
        size_t i = s.find(r);
        if ( i == string::npos ) return ret;
        s.replace(i, r.size(), to);
        ret++;
    }
    return 0;
}

int main(int ac, char *av[])
try
{
	if( ac<4 )
	{
		cout<<"Usage: in out file [more files]\n";
		return 0;
	}

    std::map<string,string> m,f;

    for( int i=3; i<ac; i++ )
    {
        string in = file2str(av[i]+string(".in"));
        m[in] = file2str(av[i]+string(".out"));
        f[in] = av[i];
    }

    string text = file2str(av[1]);
    int cnt=0;

    for( auto i : m )
    {
        int c0 = cnt;
        cnt += replaceAll(text,i.first,i.second);
        if( c0 != cnt ) cout<<f[i.first]<<'\t'<<(cnt-c0)<<'\n';
    }

    std::ofstream of(av[2], std::ios::binary);
    of<<text;

    cout<<"Number of replacements: "<<cnt<<'\n';
}
catch(string e)
{
    cout<<"Error: "<<e<<"\n";
}
catch(...)
{
    cout<<"Error\n";
}

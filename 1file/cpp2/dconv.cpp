#include <iostream>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

bool replaceOne(string & s, const string & r, const string & to)
{
    bool found = false;
    //while (1)
    {
        size_t i = s.find(r);
        if ( i == string::npos ) return found;
        s.replace(i, r.size(), to);
        found = true;
    }
    return found;
    //throw 0; // never
}

using vstr = vector<string>;
vstr str2vstr(const string & s, string delim)
{
    if ( s.size() < 1 ) return vstr();

    const string * ps = &s;
    string ds; // with delim at the end
    string::size_type i = s.rfind(delim);
    if ( i == string::npos || i + delim.size() != s.size() )
    {
        ds = s + delim;
        ps = &ds;
    }

    // now ps points to a string with delim at the end

    vstr r;
    i = 0;
    while ( 1 )
    {
        string::size_type j = ps->find(delim, i);
        if ( j == string::npos ) break;
        r.push_back(ps->substr(i, j - i));
        i = j + delim.size();
    }

    return r;
}


string convdate(string d)
{
    auto v = str2vstr(d, "/");
    if ( v.size() != 3 ) throw "bad date";

	//cout<<v[0]<<' '<<v[1]<<' '<<v[2] << '\n';

	if( v[2] == "2021" ){}
	else if( v[2] == "2020" ){}
	else if( v[2] == "2019" ){}
	else if( v[2] == "2022" ){}
	else throw "year is not 2019,2021,2020,2022 in "+d;

	for( int i=0; i<2; i++ ) while(v[i].size()<2) v[i]='0'+v[i];

	//cout<<v[0]<<' '<<v[1]<<' '<<v[2] << '\n';

    ///return "21"+v[1]+v[0];
    return v[2].substr(2,2)+v[1]+v[0];
}

tuple<size_t, size_t> finddate( const string & f)
{
    size_t i = 0, j;

    while (1)
    {
        i = f.find_first_of("0123456789", i);
        if ( i == string::npos ) return {i, i};
        j = f.find_first_not_of("0123456789/", i);
        if ( j == string::npos ) return {i, i};
        auto s = f.substr(i, j - i);

        bool z1 = ( s.find("/") != string::npos );
        replaceOne(s, "/", "-");
        bool z2 = ( s.find("/") != string::npos );
        replaceOne(s, "/", "-");
        bool z3 = ( s.find("/") == string::npos );

        if ( z1 && z2 && z3 ) break;
        i = j;
    }

    return {i, j};
}

int main()
try
{
    string file;
    for ( char c; cin.get(c); ) file += c;

    while (1)
    {
        auto [i, j] = finddate(file);
        if ( i == string::npos ) break;
        auto date = file.substr(i, j - i);
        //cout << date << '\n';
        auto mydate = convdate(date);
        replaceOne(file, date, mydate);
    }

    cout << file;
}
catch (const char *e)
{
    cout << "error: " << e << "\n";
    return 1;
}
catch (string e)
{
    cout << "error: " << e << "\n";
    return 1;
}
catch (...)
{
    cout << "exception error\n";
    return 1;
}

// program recursively renames files
// from ANY to ASCII latin letters
// Win32 MS C++ ver >15
// Oleg Mazonka 2009

#ifndef _M_X64
#else
#error Required C++ 32-bit compiler
#endif


#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

#include <direct.h>
#include <io.h>
#include <fcntl.h>

using std::map;
using std::cout;
using std::cerr;
using std::vector;
using std::string;
using std::wstring;

namespace g{
bool dryrun = true;
bool test=false;
map<int,string> table;
} // g

namespace wdir{

typedef std::map<wstring, std::pair<unsigned long, long> > msul;
msul read();

bool setd(wstring s);
wstring getd();
bool rename(wstring oldname, wstring newname);

class in{
	wstring cwd;
public:
	in(wstring s){ 
		cwd=getd(); 
		if( !setd(s) ) 
		throw wstring()+L"cannot get into [" + s +L"]"; 
	}

	~in(){
		if( !setd(cwd) ) 
		cerr << "Error: cannot return to cwd\n"; 
	}
};

} //wdir

std::wstring s2w(const std::string& s)
{
	std::wstring temp(s.length(),L' ');
	std::copy(s.begin(), s.end(), temp.begin());
	return temp;
}

std::string w2s(const std::wstring& s)
{
	std::string temp(s.length(), ' ');
	for( size_t i=0; i<s.size(); i++ )
	temp[i] = char(s[i]);
	return temp;
}

string tos(int x){ std::ostringstream s; s<<x; return s.str(); }

void runDir(wstring d);
map<int,string> populate();

int main(int ac, char *av[])
{
  try{
	g::table = populate();

	wstring cwd = wdir::getd();

	if( ac==2 && string(av[1])=="test" )
		g::test = true;
	else if( ac==2 )
	{
	        cwd = s2w(av[1]);
	}

	runDir(cwd);

	if( g::test )
		return 0;

	g::dryrun = false;

	runDir(cwd);

  }catch(string s)
  {
	cerr<<"Error: "<<s<<'\n';
	return 1;
  }
  catch(wstring s)
  {
	cerr<<"Error: "<<w2s(s)<<'\n';
	return 1;
  }
  catch(...)
  {
	cerr<<"Unknown error\n";
	return 2;
  }
}


bool wdir::setd(wstring s)
{ 
	int r = _wchdir(s.c_str()); 
	return !r;
}

wstring wdir::getd(){ 
	int size = 100;
	wchar_t * p = new wchar_t[size];
	wchar_t * r = _wgetcwd(p,size);
	while( !r ){
		delete[] p;
		if( size > 1000000L ) return L"";
		size*=2;
		p = new wchar_t[size];
		r = _wgetcwd(p,size);
	}

	wstring rr(r);
	delete[] p;
	return rr;
}

wdir::msul wdir::read()
{
 
 msul r;
 _wfinddata_t f;
 long handle;

 if ( (handle = _wfindfirst(L"*",&f) ) != -1L )

 do {
	std::pair<unsigned long, long> atr;

	atr.first = f.time_write;

	if( !(f.attrib & _A_SUBDIR)  )
		atr.second =  f.size;
	else
		atr.second =  -1L;

	wstring name(f.name);

	if( name != L"." && name != L".." ) 
		r[name] = atr;

 }while( _wfindnext(handle,&f) != -1 );

 _findclose(handle);

 return r;
}


bool isfile(wstring f);
wstring convert(wstring s);

void runDir(wstring d)
{
	wdir::in in(d);

	//cout<<"Directory: "<<w2s(d)<<'\n';

	using wdir::msul;

	msul list = wdir::read();	

	for( msul::iterator i=list.begin(); i!=list.end(); ++i )
	{
		wstring name(i->first);

		if( i->second.second == -1L )
		{
			runDir(name);
		}

		wstring newname = convert(name);

		if( name == newname )
		{
			continue;
		}

		cout<<"["<<w2s(name)<<"] -> ["<<w2s(newname)<<"] ";

		if( isfile(newname) )
		{
			throw string()+"File ["+w2s(newname)+"] exists in ["+w2s(d)+"]";
		}

		if( g::dryrun )
			cout<<"test\n";
		else if( wdir::rename(name,newname) ) 
			cout<<"OK\n";
		else 
			cout<<"FAILED\n";
	}
}

bool wdir::rename(wstring old, wstring n){
	return !::_wrename(old.c_str(), n.c_str());
}

bool isfile(wstring f)
{
	string s = w2s(f);
	std::ifstream in(s.c_str());

	bool nofile = !in;
	
	return !nofile;
}


wstring convert(wstring s)
{
	const int upper = 126;

	wstring r;

        bool good = true;

	for( size_t i=0; i<s.size(); i++ )
	{
		int code = int(s[i]);

		map<int,string>::iterator it = g::table.find(code);
		if( it != g::table.end() )
		{
			r += s2w(it->second);
			continue;
		}

		if( code <= upper )
		{
			r += s[i];
			continue;
		}

		good = false;
		r += s2w("&#");
		r += s2w(tos(int(s[i])));
		r += s2w(";");
	}

	if( good )
		return r;
		
	throw wstring()+L"Codes are not in the table\n"+r;
}


map<int,string> populate()
{
	map<int,string> r;

	string fname = "wchar2lat.map";

	std::ifstream in(fname.c_str());

	if( !in )
		throw "Cannot open "+fname;

	while(1)
	{
		int c;
		string s;
		in>>c>>s;
		if( !in )
			break;
		r[c] = s;
	}

	return r;
}



struct entry
{
	int x;
	const char * a;
};

entry tbl[] = 
{
{ 1040, "A" },
{ 1041, "B" },
{ 1042, "V" },
};

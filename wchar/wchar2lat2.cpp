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

		r += s2w("_");
		continue;

		good = false;
		r += s2w("&#");
		r += s2w(tos(int(s[i])));
		r += s2w(";");
	}

	if( good )
		return r;
		
	throw wstring()+L"Codes are not in the table\n"+r;
}


struct entry_tbl
{
	int x;
	const char * a;
};

const int SZ = 158;
extern entry_tbl g_tbl[SZ];

map<int,string> populate()
{
	map<int,string> r;


	for( int i=0; i<SZ; i++ )
	{
		r[g_tbl[i].x] = g_tbl[i].a;
	}

	return r;
}



entry_tbl g_tbl[SZ] = 
{
{ 1040, "A" },
{ 1041, "B" },
{ 1042, "V" },
{ 1043, "G" },
{ 1044, "D" },
{ 1045, "E" },
{ 1046, "Zh" },
{ 1047, "Z" },
{ 1048, "I" },
{ 1049, "J" },
{ 1050, "K" },
{ 1051, "L" },
{ 1052, "M" },
{ 1053, "N" },
{ 1054, "O" },
{ 1055, "P" },
{ 1056, "R" },
{ 1057, "S" },
{ 1058, "T" },
{ 1059, "U" },
{ 1060, "F" },
{ 1061, "H" },
{ 1062, "C" },
{ 1063, "Ch" },
{ 1064, "Sh" },
{ 1065, "Sch" },
{ 1066, "Q" },
{ 1067, "Y" },
{ 1068, "Q" },
{ 1069, "E" },
{ 1070, "Ju" },
{ 1071, "Ja" },
{ 1072, "a" },
{ 1073, "b" },
{ 1074, "v" },
{ 1075, "g" },
{ 1076, "d" },
{ 1077, "e" },
{ 1078, "zh" },
{ 1079, "z" },
{ 1080, "i" },
{ 1081, "j" },
{ 1082, "k" },
{ 1083, "l" },
{ 1084, "m" },
{ 1085, "n" },
{ 1086, "o" },
{ 1087, "p" },
{ 1088, "r" },
{ 1089, "s" },
{ 1090, "t" },
{ 1091, "u" },
{ 1092, "f" },
{ 1093, "h" },
{ 1094, "c" },
{ 1095, "ch" },
{ 1096, "sh" },
{ 1097, "sch" },
{ 1098, "q" },
{ 1099, "y" },
{ 1100, "q" },
{ 1101, "e" },
{ 1102, "ju" },
{ 1103, "ja" },
{ 1105, "jo" },
{ 1025, "Jo" },
{ 8211, "-" },
{ 8217, "'" },
{ 174, "(C)" },
{ 171, "-" },
{ 187, "-" },
{ 244, "o" },
{ 8470, "N" },
{ 252, "u" },
{ 191, "s" },
{ 246, "o" },
{ 241, "n" },
{ 243, "o" },
{ 225, "a" },
{ 209, "N" },
{ 233, "e" },
{ 1111, "i" },
{ 1108, "e" },
{ 1110, "i" },
{ 1169, "g" },
{ 8212, "-" },
{ 230, "a" },
{ 915, "g" },
{ 161, "i" },
{ 223, "b" },
{ 189, "1" },
{ 239, "i" },
{ 188, "4" },
{ 945, "a" },
{ 8745, "Q" },
{ 197, "A" },
{ 181, "m" },
{ 948, "d" },
{ 226, "a" },
{ 960, "p" },
{ 242, "o" },
{ 238, "i" },
{ 199, "C" },
{ 931, "S" },
{ 232, "e" },
{ 249, "U" },
{ 920, "O" },
{ 229, "a" },
{ 201, "E" },
{ 186, "0" },
{ 250, "U" },
{ 964, "T" },
{ 8734, "8" },
{ 776, "O" },
{ 237, "i" },
{ 170, "a" },
{ 913, "A" },
{ 932, "T" },
{ 933, "Y" },
{ 927, "O" },
{ 925, "N" },
{ 929, "R" },
{ 928, "P" },
{ 937, "O" },
{ 269, "c" },
{ 220, "U" },
{ 235, "e" },
{ 338, "E" },
{ 8482, "t" },
{ 166, "I" },
{ 162, "c" },
{ 165, "Y" },
{ 198, "A" },
{ 234, "e" },
{ 263, "c" },
{ 8230, "." },
{ 176, "o" },
{ 169, "c" },
{ 228, "a" },
{ 8221, "'" },
{ 904, "E" },
{ 957, "n" },
{ 962, "s" },
{ 935, "X" },
{ 959, "o" },
{ 961, "r" },
{ 972, "o" },
{ 954, "k" },
{ 956, "m" },
{ 951, "j" },
{ 953, "i" },
{ 973, "u" },
{ 942, "j" },
{ 965, "u" },
{ 943, "i" },
{ 922, "K" },
{ 949, "e" },
{ 958, "x" },

};

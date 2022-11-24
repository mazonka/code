#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

#include "cgi.h"

const bool LOG_QUERY = false;

using std::string;
using std::map;
using std::cin;


// UTILITIES
string sgetenv(const string&s)
{
	const char * p = std::getenv(s.c_str());
	if( p== NULL ) return "";
	return p;
}

void ssetenv(const string&s, const string&val)
{
#if defined _MSC_VER || __MINGW32__

	putenv((s+"="+val).c_str());

#elif defined unix || defined __CYGWIN32__

	setenv(s.c_str(),val.c_str(),1);

#else

#error  platform undefined: provide implementation for ssetenv

#endif
}

#ifdef _O_BINARY
void setmode2binary(FILE*f){ setmode( fileno(f), _O_BINARY ); }
#else
void setmode2binary(FILE*f){}
#endif



string readkey(string s, int& x){
	string r;
	if( s[x] == '&' ) x++;
	while( s[x] != '=' && x < (int)s.size() )
	r += s[x++];

	return r;
}

string readval(string s, int& x){
	string r;
	while( s[x] != '&' && x < (int)s.size() )
	{
	 if( s[x] == '+' ) r += ' ';
	 else if( s[x] == '%' ){
		unsigned char a = '\0';
		if( x + 2 >= (int)s.size() )
			break; // error in the string
		x++;
		if( s[x] >= '0' && s[x] <= '9' )
			a = s[x] - '0';
		if( s[x] >= 'a' && s[x] <= 'f' )
			a = s[x] - 'a' + 10;
		if( s[x] >= 'A' && s[x] <= 'F' )
			a = s[x] - 'A' + 10;
		a<<=4;
		x++;
		if( s[x] >= '0' && s[x] <= '9' )
			a += s[x] - '0';
		if( s[x] >= 'a' && s[x] <= 'f' )
			a += s[x] - 'a' + 10;
		if( s[x] >= 'A' && s[x] <= 'F' )
			a += s[x] - 'A' + 10;

		r += a;
		
	 }else r += s[x];
	 x++;
	}

	return r;
}

map<string,string> queryparse(string s)
{
	map <string,string> r;

	int x = 0;
	while(1){
		string key = readkey(s,x);
		if ( x == (int)s.size() ) break;
		string val = readval(s,++x);
		r[key] = val;
	}

	return r;
}


void queryenv(mss &env)
{
	for( mss::iterator i=env.begin(); i!=env.end(); i++ )
	{
		i->second = sgetenv(i->first);
	}
}

void extract(mss &m)
{
  string sep;

  string  pCL = getenv("CONTENT_LENGTH");
  if(pCL == "") goto end;

  getline(cin,sep);

  while(1)
  {
	string line;
	// get content
	getline(cin,line);

	string eol;
	eol += char(13);
	if( line.size() || line[line.size()-1]=='\x0D' ) eol += char(10);

	if(!cin) break;

	// get name
	string::size_type i = line.find("name=\"");
	if( i==string::npos ) throw string("no name in [")+line+"]";
	string name = line.substr(i+6);
	i = name.find("\"");
	if( i==string::npos ) throw string("no closing in [")+line+"]";
	name = name.substr(0,i);

	while(1)
	{
		getline(cin,line);
		if(!cin) goto end;
		if( line=="" || line=="\x0D" ) break;
	}

	while(1)
	{
		getline(cin,line);
		if(!cin) goto end;
		if( line.substr(0,sep.size()-1)==sep.substr(0,sep.size()-1) ) break;
		m[name] += line;
	}

	string &mm = m[name];
	if( mm.size() && mm[mm.size()-1] == '\x0D' ) mm.erase(mm.size()-1);

	if( line!=sep ) break;

  }

end:
	;

}

void query(mss &get, mss &post)
{
  string s;

  // GET
  s = sgetenv("QUERY_STRING");

  if( LOG_QUERY && s!="" )
  {
	std::ofstream of("query_get.log");
	if( !!of ) of<<s;
  }

  get = queryparse(s);
  s="";

  setmode2binary( stdin );

  string contype  = sgetenv("CONTENT_TYPE");
  if( contype.substr(0,9)=="multipart" )
  {
	extract(post);
	return;
  }

  string ispost = sgetenv("REQUEST_METHOD");

  if( ispost!="POST" && ispost!="post" ) return;

  // POST
  {
      string  pCL = sgetenv("CONTENT_LENGTH");
      if(pCL != "")
      {	
	int   nCL = atoi(pCL.c_str());
	if(nCL > 0)
	{
		for( int i=0; i<nCL; i++ ){
	                int a = fgetc(stdin);
	                if( a==EOF) break;
	                if( feof(stdin) ) break;
	                s += char(a);
		}
         }
      }

  }

  if( LOG_QUERY && s!="" )
  {
	std::ofstream of("query_post.log");
	if( !!of ) of<<s;
  }

  post = queryparse(s);

}

cgi::cgi()
{
	query(get,post);
	REMOTE_ADDR = sgetenv("REMOTE_ADDR");
	if( REMOTE_ADDR=="" ) REMOTE_ADDR = sgetenv("REMOTE_HOST");

	HTTP_X_FORWARDED_FOR = sgetenv("HTTP_X_FORWARDED_FOR");

	IP = REMOTE_ADDR;
	if( HTTP_X_FORWARDED_FOR!="" ) IP=HTTP_X_FORWARDED_FOR;
}

string cgi::operator[](const string &s) const
{
	mss::const_iterator i = post.find(s);
	if( i!=post.end() ) return i->second;

	i = get.find(s);
	if( i!=get.end() ) return i->second;

	return "";
}

void cgi::cl(int ac, char *av[])
{
	if( ac > 1 ) ssetenv("QUERY_STRING",av[1]);
}


string cgi::dump() const
{
  string s;
  s = "<hr>\nGET<br>";

  for( mss::const_iterator i=get.begin(); i!=get.end(); i++ )
  s+=i->first+" = ["+i->second+"]<br>\n";

  s+="<hr>\nPOST<br>";

  for( mss::const_iterator i=post.begin(); i!=post.end(); i++ )
  s+=i->first+" = ["+i->second+"]<br>\n";

  s+="<hr><br>\n";

  return s;

}

string file2str(string fname)
{
	std::ifstream in(fname.c_str());
	string sin;
	while(1)
	{
		char a='\0';
		in.get(a);
		if( !in ) break;
		sin += a;
	}
	return sin;
}

bool mainhtml0(void (*f)()){

 int r = 0;

 try{
	//std::cout<<"Content-Type: text/html\n\n";
	std::cout<<"Content-Type: text/html; charset=WINDOWS-1251\n\n";

 	f();

 }catch(string s){
	std::cout<<s<<std::endl;
 	r = 1;
 }catch(const char *s){
	std::cout<<s<<std::endl;
 	r = 2;
 }catch(std::exception e){
	std::cout<<"Std exception ["<<e.what()<<"]"<<std::endl;
 	r = 3;
 }catch(...){
	std::cout<<"Unknown exception"<<std::endl;
 	r = 4;
 }

 return !r;
}


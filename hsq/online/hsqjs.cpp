#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>

#include "cgi.h"

using std::cout;
using std::vector;

void xmain();
int main(int ac, char *av[])
{
	cgi::cl(ac,av);
	mainhtml0(xmain);
}

// forwards
namespace ut { void replaceAll(string &s, const string &r, const string &to); }
namespace driver { string getlogo(); }

string compile_hsq2asq(const string &s);
string compile_asq2sq(const string &s);

string hw = 
	"12 12 3 36 37 6 37 12 9 37 \n"
	"37 12 0 -1 15 38 36 18 12 12 \n"
	"21 52 37 24 37 12 27 37 37 \n"
	"30 36 12 -1 37 37 0 39 0 -1 \n"
	"72 101 108 108 111 44 32 87 \n"
	"111 114 108 100 33 52";

void logtime(const string &ip, const string &xip)
{
  try{

  std::time_t tmt = std::time(0);
  struct std::tm * ptm = std::localtime(&tmt);
  char buf[200];
  std::strftime(buf,200,"%d %b %Y %H:%M:%S",ptm);
  string stime = buf;

  std::ofstream of("hsqjs.log",std::ios::app);
  of<<ip<<" ("<<xip<<") "<<stime<<'\n';

  }catch(...){}
}

void xmain()
{
  cgi c;

  string cmd;

  string file = file2str("hsqjs.ct");

  string act = c["act"];
  string text = c["text"];

  logtime(c.REMOTE_ADDR,c.HTTP_X_FORWARDED_FOR);

  try{

  if( act == "" )
  {
	ut::replaceAll(file,"$PRIVET",driver::getlogo());
	ut::replaceAll(file,"$TEXT",hw);
  }
  else if( act == "HSQ -> ASQ" )
  {
	string result = compile_hsq2asq(text);
	ut::replaceAll(file,"$PRIVET",act+": OK");
	ut::replaceAll(file,"$TEXT",result);
  }
  else if( act == "ASQ -> SQ" )
  {
	string result = compile_asq2sq(text);
	ut::replaceAll(file,"$PRIVET",act+": OK");
	ut::replaceAll(file,"$TEXT",result);
  }
  else if( act == "HSQ -> SQ" )
  {
	string result = compile_asq2sq(compile_hsq2asq(text));
	ut::replaceAll(file,"$PRIVET",act+": OK");
	ut::replaceAll(file,"$TEXT",result);
  }
  else
  {
	ut::replaceAll(file,"$PRIVET","ERROR: action ["+act+"] not defined");
	ut::replaceAll(text,"\r","");
	ut::replaceAll(file,"$TEXT",text);
  }

  }catch(string s){

	ut::replaceAll(file,"$PRIVET","ERROR: "+s);
	ut::replaceAll(text,"\r","");
	ut::replaceAll(file,"$TEXT",text);

  }catch(...){
	ut::replaceAll(file,"$PRIVET",
		"Unknown exception! It means bug in the compiler. Please, "
		"send the test input to <b>mazonka@gmail.com</b> Thank you."
	);

	ut::replaceAll(text,"\r","");
	ut::replaceAll(file,"$TEXT",text);
  }

  cout<<file<<'\n';
}

namespace hsqcomp{
string hsqcompile(vector<string> &vname, vector<string> &vtext);
string itrcompile(vector<string> &vname, vector<string> &vtext);
} // compiler

namespace asqcomp{
string asqcompile(vector<string> &vtext);
} // asqcomp

namespace emulator{
void sqemulate(vector<string> &vtext, bool big);
} // emulator


string compile_hsq2asq(const string &s)
{
	const int SZMAX = 100000;
	if( s.size()>SZMAX )
		throw string()+"HSQ: Input program is too long for the online compiler.";

	vector<string> nm, t;
	nm.push_back("online.hsq");
	t.push_back(s);

	string r = hsqcomp::hsqcompile(nm,t);
	if( r.size()>SZMAX )
		throw string()+"HSQ: Output program is too long for the online compiler.";

	return r;
}

string compile_asq2sq(const string &s)
{
	const int SZMAX = 100000;
	if( s.size()>SZMAX )
		throw string()+"ASQ: Input program is too long for the online compiler.";

	vector<string> t;
	t.push_back(s);

	string r = asqcomp::asqcompile(t);
	if( r.size()>SZMAX )
		throw string()+"ASQ: Output program is too long for the online compiler.";

	return r;
}

#define main main_dummy
#include "../hsq.cpp"

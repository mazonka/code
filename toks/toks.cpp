#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "../plib/readdir.cpp"

using namespace std;

bool load(ifstream & is, string & v){

        char c;
	bool bin = false;

        while(1){
                is.get(c);
                if( !is ) break;
                v += c;

		if( c=='\t' || c=='\n' || c=='\r' || (c>=' '&&c<='~') ){}
		else bin = true;
        }

        return !bin;
}

struct fileline
{
	string file;
	int line;

	bool operator<(const fileline&x) const
	{
		if( file<x.file ) return true;
		if( x.file<file ) return false;
		return line<x.line;
	}

	fileline(string f, int l): file(f), line(l){}
};

bool getln(string &s, int & pos, string &out);
bool gettk(string &s, int & pos, string &out);
void writetok(ostream &o,  map<fileline,string> & tbl, const string &name);
void writefile(ostream &o, const string &name, string &file);

string hashf(const string &x)
{
	int i=0;
	for( int j=0; j<x.size(); j++ )
	{
		i += 13*(unsigned int)((unsigned char)x[j]);
		i = i%997;
	}
	char b[20];
	sprintf(b,"%d",i);
	return b;
}

string tohtml(const string &s);

int main(int ac, char *av[])
{
  // step 1 receive all files
  msul fls = readdir();

  map<string,string> mfls;

  for( msul::iterator i=fls.begin(); i!=fls.end(); i++ )
  {
	if( i->second.second < 0 ) continue;

	ifstream in(i->first.c_str());
	if( !in )
	{
		cout<<"ERROR: cannot open "<<i->first<<'\n';
		continue;
	}

	string file;
	cout<<i->first;
	if( !load(in,file) )
	{
		cout<<"\tbinary file - IGNORED\n";
		continue;
	}
	cout<<"\tloaded\n";

	mfls[i->first] = file;
  }

  // step 2 files loaded
  map<string,map<fileline,string> > toks;

  for( map<string,string>::iterator i=mfls.begin(); i!=mfls.end(); ++i )
  {
	int pos=0, linenum=0;
	string ln;
	while( getln(i->second,pos,ln) )
	{
//cout<<"getln "<<i->first<<':'<<ln<<'\n';

		++linenum;
		int pos2=0;
		string tk;
		while( gettk(ln,pos2,tk) )
		{
			toks[tk][fileline(i->first,linenum)] = ln;
		}
	} 
  }

  dir::make("html");
  dir::make("html/file");
  dir::make("html/toks");

  // dump toks
  for( map<string,map<fileline,string> >::iterator i=toks.begin(); i!=toks.end(); ++i )
  {

	string ofl = string("html/toks/")+i->first+"-"+hashf(i->first)+".html";
	ofstream of( ofl.c_str());

	if( !of )
	{
		cout<<"ERROR cannot open file "<<ofl<<'\n';
		return 2;
	}

	writetok(of,i->second,i->first);
  }


  // dump files
  for( map<string,string>::iterator i=mfls.begin(); i!=mfls.end(); ++i )
  {
	string ofl = string("html/file/")+i->first+".html";
	ofstream of( ofl.c_str());

	if( !of )
	{
		cout<<"ERROR cannot open file "<<ofl<<'\n';
		return 2;
	}

	writefile(of,i->first,i->second);
  }

  // write main index file
  ofstream ind("html/index.html");
  if( !ind )
  {
	cout<<"ERROR cannot open html/index.html\n";
	return 3;
  }

  ind<<"<html>\n";
  ind<<"<b>\n";
  for( map<string,string>::iterator i=mfls.begin(); i!=mfls.end(); ++i )
  {
	string ofl = string("file/")+i->first+".html";
	ind<<"<a href='"<<ofl<<"'>"<<i->first<<"</a>&nbsp;&nbsp;\n";
  }
  ind<<"</b><hr>\n";

  for( map<string,map<fileline,string> >::iterator i=toks.begin(); i!=toks.end(); ++i )
  {

	string ofl = string("toks/")+i->first+"-"+hashf(i->first)+".html";
	ind<<"<a href='"<<ofl<<"'>"<<i->first<<"</a>&nbsp;&nbsp;\n";
  }

  ind<<"\n</html>\n";
}


bool getln(string &s, int & pos, string &out)
{
	if( pos==string::npos ) return false;

	string::size_type k = s.find("\n",pos);

	if( k!=string::npos )
	{
		out = s.substr(pos,k-pos);
		pos = k+1;
	}
	else
	{
		out = s.substr(pos);
		pos = k;
	}

	return true;
}

int gettokstart(string &s, int i)
{
	while(1)
	{
		if( s[i] == '_' ) break;
		if( s[i] >= 'A' && s[i]<='Z' ) break;
		if( s[i] >= 'a' && s[i]<='z' ) break;
	
		if( ++i >= s.size() ) break;
	}
	return i;
}

bool gettk(string &s, int & i, string &out)
{
	if( i >= s.size() ) return false;

	i = gettokstart(s,i);
	if( i >= s.size() ) return false;

	out="";
	while(1)
	{
		out += s[i++];
		if( i >= s.size() ) break;
		if( s[i] == '_' ) continue;
		if( s[i] >= 'A' && s[i]<='Z' ) continue;
		if( s[i] >= 'a' && s[i]<='z' ) continue;
		if( s[i] >= '0' && s[i]<='9' ) continue;
		break;
	}

	return true;
}

bool gettkornot(string &s, int & i, string &out, bool * ftk = 0)
{
	if( i >= s.size() ) return false;
	int j = gettokstart(s,i);

	if( ftk ) *ftk = true;

	if( j==i ) return gettk(s,i,out);

	if( ftk ) *ftk = false;

	out = s.substr(i,j-i);
	i=j;
	return true;
}

void writetok(ostream &o,  map<fileline,string> & tbl, const string &name)
{
  o<<"<html><h1>"<< name<<"</h1>\n<table border=0>\n";

  for( map<fileline,string>::iterator j=tbl.begin(); j!=tbl.end(); ++j )
  {
	o<<"<tr><td>"<<j->first.file<<"</td>";
	o<<"<td><a href='../file/"<<j->first.file<<".html#"<<j->first.line<<"'>"<<j->first.line<<"</a></td>";
	o<<"<td><tt>";
	//o<<tohtml(j->second);
		int pos2=0;
		string tk;
		bool ftk;
		while( gettkornot(j->second,pos2,tk,&ftk) )
		{
			if( ftk && tk==name )
				o<<"<b>"<<tohtml(tk)<<"</b>";
			else
				o<<tohtml(tk);
		}
	o<<"</tt></td></tr>";
  }

  o<<"</table></html>\n";
}

string tohtml(const string &s)
{
	string r;
	for( int i=0; i<s.size(); i++ )
	{
		if( s[i] == '&' ) r+="&amp;";
		else if( s[i] == '<' ) r+="&lt;";
		else if( s[i] == '>' ) r+="&gt;";
		else r+=s[i];
	}

	return r;
}

void writefile(ostream &o, const string &name, string &file)
{

	o<<"<html><table><tr><td><h1>"<<name<<"</h1></td><td>"
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href='../index.html'>files</a></td></tr></table>\n";

	o<<"<pre>\n";

	int pos=0, linenum=0;
	string ln;
	while( getln(file,pos,ln) )
	{
		++linenum;

		o<<"<a name='"<<linenum<<"'></a>"<<linenum<<"   ";
		int pos2=0;
		string tk;
		bool ftk;
		while( gettkornot(ln,pos2,tk,&ftk) )
		{
			if( ftk )
				o<<"<a href='../toks/"<<tk<<"-"<<hashf(tk)<<".html'>"<<tohtml(tk)<<"</a>";
			else
				o<<tohtml(tk);
		}
		o<<"\n";
	} 

	o<<"</pre>\n</html>\n";
}


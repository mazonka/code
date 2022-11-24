#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>

#include "../plib/readdir.cpp"
#include "../plib/transl.cpp"

using std::string;
typedef std::vector<string> vstr;
using std::cout; 

namespace g{

std::ofstream out;
string cwd;
vstr tags;
vstr ignore;
vstr path;
const char metaname[] = { "stb.meta" };
const char cfgname[] = { "stb.cfg" };
const char outname[] = { "stb.csv" };

} // g

struct StbInfo
{
	string name;
	string type; // d/f/m
	long double size;
	string ext;
	vstr path;
	vstr tags;
	vstr names;
	string unk;

	StbInfo(): size(0) {}
};

double item(const string &name, bool record, double size, bool dir);
void probecfg();

int main(int ac, char *av[])
try{
	// probe cfg
	g::cwd = dir::getd();
	probecfg();
	if( !dir::setd(g::cwd) )
	{
		std::cerr<<"Cannot set dir back to ["<<g::cwd<<"]\n";
		return 1;
	}

	g::out.open(g::outname);

	// write header
	g::out<<"name,type,size,ext,path";
	for( size_t i=0; i<g::tags.size(); i++ )
		g::out<<","<<g::tags[i];
	g::out<<",unknown\n";

	string start = ".";
	if( ac>1 ) start = av[1];

	double sz = item(start,true,0,true);

	cout<<"Size = "<<std::setprecision(20)<<sz<<'\n';

}catch(...)
{
 	std::cerr<<"Unknwon error\n";
	return 2;
}


void readNames(std::istream &is, vstr &x)
{	
	while(1)
	{
		string s;
		is>>s;
		if( !is ) break;
		x.push_back(s);
	}
}


void probecfg()
{
	std::ifstream in(g::cfgname);

	if( !in )
	{
		string wd = dir::getd();
		bool fail = !dir::setd("..");
		bool same = (dir::getd()==wd);
		if( fail || same )
		{
			cout<<"Cannot find "<<g::cfgname<<" file - using empty tags\n";
			return;
		}
		cout<<"Trying cfg in "<<dir::getd()<<'\n';
		probecfg();
		return;
	}

	
	while(1)
	{
		string line;
		std::getline(in,line);
		if( !in ) break;
		if( line=="" ) continue;
		std::istringstream is(line);
		string n;
		is>>n;
		if( n=="TAGS" ) readNames(is,g::tags);
		else if( n=="IGNORE" ) readNames(is,g::ignore);
		else
		{
			cout<<"Error: line ["<<line<<"] in cfg is unknown\n";
		}
	}

	cout<<"Config found at ["<<dir::getd()<<"]: "
		<<g::tags.size()<<" tags used\n";
}


void recordFile(string name, double size);
void recordDir(string name, double size);
void recordMeta(string name, double size);

double item(const string &name, bool rec, double sz, bool dir)
{
	//cout<<"AAA item "<<name<<' '<<(rec?"r":"v")<<' '<<sz<<' '<<(dir?"d":"f")<<'\n';

	if( !dir )
	{
		if( rec ) recordFile(name,sz);
		return sz;
	}

	// directory
	dir::in entering(name);
	g::path.push_back(name);

	msul list = readdir();

	bool meta = ( list.find(g::metaname) != list.end() );

	if( meta ) rec = false;

	double size = 0;

	for( msul::iterator i=list.begin(); i!=list.end(); ++i )
	{
		string iname = i->first;
		long lsz = i->second.second;

		if( g::ignore.end() != 
			std::find(g::ignore.begin(),g::ignore.end(),iname) )
			continue;

		if( lsz == -1L ) size += item(iname,rec,0,true);
		else size += item(iname,rec,lsz,false);
	}


	if( meta ) recordMeta(name,size);

	g::path.pop_back();

	if( rec ) recordDir(name,size);

	return size;
}


string strip(const string&s);

string buildPath(const vstr &path)
{
	string r;
	for( size_t j=0; j<path.size(); j++ )
	{
		if( path[j] == "." ) continue;
		r+=path[j];
		if( j+1<path.size() ) r+="/";
	}
	return r;
}

void recordInfo(const StbInfo &i)
{
	g::out<<strip(i.name)<<','<<i.type<<',';
	g::out<<std::setprecision(20)<<i.size<<','<<strip(i.ext)<<',';

	g::out<<buildPath(i.path)<<',';

	for( size_t j=0; j<g::tags.size(); j++ )
	{
		if( j < i.tags.size() ) g::out<<i.tags[j];
		g::out<<',';
	}

	g::out<<strip(i.unk)<<'\n';
}

string strip(const string&s)
{
	string r;
	for( int i=0; i<s.size(); i++ )
	{
		if( s[i] == '<' ) continue;
		if( s[i] == ',' ) continue;
		if( s[i] == '\n' ) continue;
		if( s[i] == '\r' ) continue;
		r += s[i];
	}

	return r;
}


void recordFile(string name, double size)
{
	StbInfo r;
	r.name = name;
	r.type = "f";
	r.size = size;

	string::size_type i = name.rfind(".");
	if( i!=string::npos )
	{
		r.ext = name.substr(i+1);
		r.name = name.substr(0,i);
	}

	r.path = g::path;

	recordInfo(r);
}

void recordDir(string name, double size)
{
	StbInfo r;
	r.name = name;
	r.type = "d";
	r.size = size;
	r.path = g::path;

	recordInfo(r);
}

int getTagIndex(string tag);

void recordMeta(string name, double size)
{
	std::ifstream in(g::metaname);

	if( !in )
	{
		cout<<"Cannot read meta '"<<g::metaname
			<<"' in ["<<buildPath(g::path)<<"]\n";
		return;
	}

	StbInfo r;

	r.tags.resize( g::tags.size() );

	r.size = size;
	r.path = g::path;

	vstr names;

	while(1)
	{
		string line;
		std::getline(in,line);
		if( !in ) break;
		std::istringstream is(line);
		string tag;
		is>>tag;
		string value = strip(line.substr(tag.size()+1));

		if( tag=="name" )
		{
			names.push_back(translit(value));
			continue;
		}

		int idx = getTagIndex(tag);
		if( idx<0 )
		{
			r.unk += "["+strip(line)+"] ";
			continue;
		}

		r.tags[idx] = value;
	}

	if( names.empty() )
	{
		r.name = name;
		recordInfo(r);
		return;
	}

	for( int i=0; i<names.size(); i++ )
	{
		r.name = names[i];
		recordInfo(r);
	}
}

int getTagIndex(string tag)
{
	for( int i=0; i<g::tags.size(); i++ )
	if( g::tags[i] == tag ) return i;
	return -1;
}


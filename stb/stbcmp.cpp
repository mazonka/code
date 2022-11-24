
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <set>
#include <map>
#include <cstdlib>

#include "../plib/readdir.cpp"

using std::string;
typedef std::vector<string> vstr;
using std::cout; 

struct File
{
	string name;
	double size;
	File(const string &s, double sz):name(s),size(sz){}
	bool operator<(const File&f) const 
	{
		if( name < f.name ) return true;
		if( f.name < name ) return false;
		return size < f.size;
	}
};

typedef std::vector<File> vfile;

namespace g{

vstr path;
string cwd;
vfile files1;
vfile files2;
vfile same;
vstr ignore;
bool nozero = false;

} // g

void main_same(const string &d, int size);
void main_diff(const string &d1, const string &d2);
void main_deep(const string &d1, const string &d2);
void main_copy(const string &d1, const string &d2);
void dumpDiff(const string &d1, const string &d2);
void readIgnore(const char *file);

int main(int ac, char *av[])
try{
	if( ac<3 )
	{
	cout<<"Usage:\n";
	cout<<"stbcmp diff dir1 dir2 - quick diff 2 directories\n";
	cout<<"stbcmp same dir1 [options] - finds groups of equal files\n";
	cout<<"stbcmp deep dir1 dir2 - diff 2 dirs with reading files\n";
	cout<<"stbcmp copy dir1 dir2 - diff and copy from dir1 to dir2\n";
	cout<<"1: Only 'copy' can modify data on disk\n";
	cout<<"2: Try using only forward slashes\n";
	cout<<"3: Equal files may not be the same in pairs, e.g. ab ac db dc\n";
	cout<<"4 same: -i ignore is a file with a list (1 per line) of names to ignore\n";
	cout<<"5 same: -s len is first Kbytes to check; 0 (default) for whole files\n";
	cout<<"6 same: -z do not report zero size files\n";
	cout<<"7 same: e.g. stbcmp same dir -i file -z -s 100\n";
	return 0;
	}

	if( string(av[1])=="same" )
	{
		int size = 0;
		for( int i=3; i<ac; i++ )
		{
		  if( string("-z")==av[i] ) g::nozero = true;
		  else if( string("-s")==av[i] && i+1<ac ) size = std::atoi(av[++i]);
		  else if( string("-i")==av[i] && i+1<ac ) readIgnore(av[++i]);
		  else { main(0,0); return 1; }
		}

		main_same(av[2],size); 
		return 0; 
	}

	if( ac<4 ) return 1;

	if( string(av[1])=="diff" ) main_diff(av[2],av[3]);
	else if( string(av[1])=="copy" ) main_copy(av[2],av[3]);
	else if( string(av[1])=="deep" ) main_deep(av[2],av[3]);
	else
	{
		cout<<"Wrong usage\n";
		return 10;
	}

	dumpDiff(av[2],av[3]);

	return 0;

}catch(string err){
	cout<<"\nError: "<<err<<"\n";
	return 2;
}catch(...){
	cout<<"\nUnknown error\n";
	return 3;
}

string buildPath(const vstr &path)
{
	string r;
	for( size_t j=0; j<path.size(); j++ )
	{
		if( path[j] == "." ) continue;
		r+=path[j]+"/";
	}
	return r;
}


void item(vfile &vf, const string &name, bool dir, double sz)
{
	if( !dir )
	{
		if( g::nozero && sz==0 ) {} else
		vf.push_back( File( buildPath(g::path)+name, sz ) );
		return;
	}

	// directory
	dir::in entering(name);
	g::path.push_back(name);

	msul list = readdir();

	for( msul::iterator i=list.begin(); i!=list.end(); ++i )
	{
		string iname = i->first;
		long lsz = i->second.second;

		if( g::ignore.end() != 
			std::find(g::ignore.begin(),g::ignore.end(),iname) )
			continue;

		if( lsz == -1L ) item(vf,iname,true,0);
		else item(vf,iname,false,lsz);
	}


	g::path.pop_back();
}


void getFiles2(const string &d1, const string &d2)
{
	{
		cout<<"Scan "<<d1<<std::flush;
		dir::in dummy(d1);
		item(g::files1,".",true,0);
		cout<<" - "<<g::files1.size()<<" files\n";
	}

	{
		cout<<"Scan "<<d2<<std::flush;
		dir::in dummy(d2);
		item(g::files2,".",true,0);
		cout<<" - "<<g::files2.size()<<" files\n";
	}
}

void analyse();

void main_diff(const string &d1, const string &d2)
{
        getFiles2(d1,d2);

	cout<<"Quick analysis: "<<std::flush;

	analyse();

	cout<<"\tEqual files: "<<g::same.size()
	<<"\tIn dir1: "<<g::files1.size()
	<<"\tIn dir2: "<<g::files2.size()<<'\n';
}

void analyse()
{
	//dumpDiff("","");

	std::set<File> sfiles1(g::files1.begin(),g::files1.end());
	std::set<File> sfiles2(g::files2.begin(),g::files2.end());

	g::same.clear();
	std::set_intersection(
		sfiles1.begin(),sfiles1.end(),
		sfiles2.begin(),sfiles2.end(),
		std::back_inserter(g::same)
	);
	
	g::files1.clear();
	std::set_difference(
		sfiles1.begin(),sfiles1.end(),
		sfiles2.begin(),sfiles2.end(),
		std::back_inserter(g::files1)
	);
	
	g::files2.clear();
	std::set_difference(
		sfiles2.begin(),sfiles2.end(),
		sfiles1.begin(),sfiles1.end(),
		std::back_inserter(g::files2)
	);
}


void dumpFiles(const string &d, const vfile &vf, bool sz=true)
{
	for( int i=0; i<vf.size(); i++ )
	{
		//cout<<d<<'/';
		cout<<vf[i].name;
		if(sz) cout<<" ("<<std::setprecision(20)<<vf[i].size<<")";
		cout<<"\n";
	}
}

void dumpDiff(const string &d1, const string &d2)
{
	if( !g::same.empty() )
		cout<<"\nSame files in both directories: "<<g::same.size()<<"\n";

	if( !g::files1.empty() )
	{
		cout<<"\nFiles in ["<<d1<<"] that are not present or different in ["
			<<d2<<"]\n";
		dumpFiles(d1,g::files1);
	}

	if( !g::files2.empty() )
	{
		cout<<"\nFiles in ["<<d2<<"] that are not present or different in ["
			<<d1<<"]\n";
		dumpFiles(d2,g::files2);
	}

	if( g::files1.empty() && g::files2.empty() )
		cout<<"Files compared OK\n";
}

void copyFile(const string &d1, const string &d2, const string &n);

void main_copy(const string &d1, const string &d2)
{
	main_diff(d1,d2);

	int sz = g::files1.size();
	for( int i=0; i<sz; i++ )
	{
		cout<<(i+1)<<'/'<<sz<<' '<<g::files1[i].name<<std::flush;
		copyFile(d1,d2,g::files1[i].name);
		cout<<" - copy ok\n";
	}

	g::files1.clear();
}

void copyFile(const string &d1, const string &d2, const string &n)
{
	string n1 = d1+"/"+n;
	string n2 = d2+"/"+n;

	std::ifstream in(n1.c_str(),std::ios::binary);
	if( !in ) throw "Cannot open ["+n1+"] for reading";

	std::ofstream of(n2.c_str(),std::ios::binary);
	if( !of ) throw "Cannot open ["+n2+"] for writing";

	const int SZ = 10000;
	char buf[SZ];

	while(1)
	{
		in.read(buf,SZ);
		int bsz = in.gcount();
		if( bsz < 1 ) break;

		of.write(buf,bsz);

		if( !in ) break;
		if( !of ) throw "Cannot write to ["+n2+"]";
	}

}

double compFile(const string &d1, const string &d2, const string &n);

void main_deep(const string &d1, const string &d2)
{
	main_diff(d1,d2);

	int sz = g::same.size();
	for( int i=0; i<sz; i++ )
	{
		cout<<(i+1)<<'/'<<sz<<' '<<g::same[i].name<<std::flush;
		double off = compFile(d1,d2,g::same[i].name);
		if( off < 0 )
		{
			cout<<" - ok\n";
			continue;
		}
		cout<<" - FAILED:"<<off<<"\n";

		g::files1.push_back(g::same[i]);
		g::files2.push_back(g::same[i]);
	}
}


double compFile(const string &d1, const string &d2, const string &n)
{
	string n1 = d1+"/"+n;
	string n2 = d2+"/"+n;

	std::ifstream in1(n1.c_str(),std::ios::binary);
	if( !in1 ) throw "Cannot open ["+n1+"] for reading";

	std::ifstream in2(n2.c_str(),std::ios::binary);
	if( !in2 ) throw "Cannot open ["+n2+"] for reading";

	const int SZ = 10000;
	char buf1[SZ];
	char buf2[SZ];

	double off=0;

	while(1)
	{
		in1.read(buf1,SZ);
		int bsz1 = in1.gcount();

		in2.read(buf2,SZ);
		int bsz2 = in2.gcount();

		if( bsz1 != bsz2 || (!in1&&in2) || (in1&&!in2) ) 
		throw "Files ["+n1+"] and ["+n2+"] are corrupted or of differen sizes";

		if( bsz1 < 1 ) break;

		for( int i=0; i<bsz1; i++ ) if( buf1[i]!=buf2[i] ) return off+i;

		if( !in1 || !in2 ) break;

		off += bsz1;
	}

	return -1;
}

void compFilesN(vstr &v, vstr &p, const string &d, double size);

void main_same(const string &d, int size)
{
	{
		cout<<"Scan "<<d<<std::flush;
		dir::in dummy(d);
		item(g::files1,".",true,0);
		cout<<" - "<<g::files1.size()<<" files\n";
	}


	std::map<double,vstr> groups;

	int sz = g::files1.size();
	for( int i=0; i<sz; i++ )
	{
		groups[g::files1[i].size].push_back(g::files1[i].name);
	}

	std::map<double,vstr> g2;

	for( std::map<double,vstr>::iterator i=groups.begin();
		i != groups.end(); ++i )
	{
		if( i->second.size()==1 ) continue;
		g2[i->first] = i->second;
	}

	g::files1.clear();

	for( std::map<double,vstr>::iterator i=g2.begin();
		i != g2.end(); ++i )
	{
		vstr &v = i->second;
		vstr p;
	
		compFilesN(v,p,d,1024*size);

		int sz = v.size();

		std::ostringstream os;
		os<<"= "<<std::setprecision(20)<<i->first<<" =";

		if( sz )
		{
			g::files2.push_back( File(os.str(),0) );
			for( int j=0; j<sz; j++ )
				g::files2.push_back( File(v[j],0) );
			g::files2.push_back( File("= = = =\n",0) );
		}

		sz = p.size();

		if( sz )
		{
			g::files1.push_back( File(os.str(),0) );
			for( int j=0; j<sz; j++ )
				g::files1.push_back( File(p[j],0) );
			g::files1.push_back( File("= = = =\n",0) );
		}
	}

	if( g::files2.empty() ) cout<<"\nNo same files found\n";
	else
	{
		cout<<"\nEqual files found\n\n";
		dumpFiles(d,g::files2,false);
	}

	if( g::files1.empty() );
	else
	{
		cout<<"\nPotentially equal files found";
		if( size ) cout<<" (only first "<<size<<"Kb checked)";
		cout<<"\n\n";
		dumpFiles(d,g::files1,false);
	}
}

void cfn_read(std::istream &is, string &dest, int bsize)
{
	char * buf = new char[bsize+1];
	is.read(buf,bsize);

	int sz = is.gcount();
	string x(buf,sz);

	dest.swap(x);
	delete [] buf;
}

void compFilesN(vstr &v, vstr &p, const string &d, double size)
{
	const bool dbg = false;

	const int BSZ = 10000;

	int sz = v.size();
	if( sz < 2 ) return;

	if( dbg )
	{
		cout<<"Comparing ";
		for( int i=0; i<sz; i++ ) cout<<"["<<v[i]<<"] ";
	}

	std::vector<std::ifstream*> in(sz);
	std::vector<bool> go(sz);
	std::vector<bool> same(sz);
	std::vector<string> buf(sz);

	// init
	for( int i=0; i<sz; i++ )
	{
		go[i] = true;
		same[i] = false;
		string n = (d+"/"+v[i]);

	
		in[i] = new std::ifstream( n.c_str(), std::ios::binary );

		std::ostringstream os; os<<i;
		if( !*in[i] ) throw "Cannot open "+os.str()+"-th ["+n+"] for reading. "
		"Probably too many files of the same size";
	}

	double cntr=0;
	while(1)
	{
		// read byte
		for( int i=0; i<sz; i++ )
			if( go[i] ) cfn_read(*in[i],buf[i],BSZ);

		int finished=0;
		for( int i=0; i<sz; i++ ) 
			if( go[i] && !*in[i] && buf[i].empty() ) ++finished;

		if( finished ) 
		{
			for( int j=0; j<sz; j++ ) 
			if( go[j] && !*in[j] && buf[j].empty() )
			{
				if( finished > 1 ) same[j] = true;
				go[j] = false;
			}
		}

		// select node
		for( int i=0; i<sz; i++ ) if( go[i] )
		{
			bool compares=false;
			for( int j=0; j<sz; j++ ) if( go[j] )
			{
				if( i==j ) continue;
				if( buf[i] == buf[j] ) compares=true;
			}
			if( !compares )	go[i] = false;
		}

		bool goall = false;
		for( int i=0; i<sz; i++ ) if( go[i] ) goall = true;

		if( !goall ) break;

		cntr += BSZ;
		if( size>0 && cntr > size ) break;

		if( dbg )
		{
			static unsigned ccc=0;
			if( !(++ccc%(1024*1024)) ) cout<<"."<<std::flush;
		}
	}

	// deinit
	for( int i=0; i<sz; i++ ) delete in[i];

	vstr r;
	for( int i=0; i<sz; i++ ) if( same[i] )	r.push_back(v[i]);
	for( int i=0; i<sz; i++ ) if( go[i] )	p.push_back(v[i]);
	v.swap(r);

	if( dbg ) cout<<'\n';

}

void readIgnore(const char *file)
{
	std::ifstream in(file);

	if( !in )
	throw "Cannot open file ["+string(file)+"] for reading";

	while(1)
	{
		string line;
		std::getline(in,line);
		if( !in ) break;
		if( line != "" ) g::ignore.push_back(line);
	}
}



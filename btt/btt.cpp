/*
	Copyright 2004 Mozaika Pty Ltd
	btt.cpp
	This program traslates binary files into
	text btt files and btt back to original
	file.
	Usage: 
		btt make aaa.bin [aaa.btt]
	or
		btt extr aaa.btt
*/

#include <string>
#include <fstream>
#include <iostream>

using std::string;

namespace btt{

string genbttname(string);
int make(string from, string to);
int extr(string from);

} //namespace

int main(int ac, char *av[])
{
	if( ac<3 )
	{
		std::cout<<"Use: 'btt make aaa.bin [aaa.btt]'"
			" or 'btt extr aaa.btt'\n";
		return 0;
	}

	if( string(av[1]) != "make" && string(av[1]) != "extr" )
	{
		std::cout<<"Use: 'make' or 'extr' commands\n";
		return 1;
	}

	int e=0;

	if( string(av[1]) == "make" )
	{
	  string oname;
	  if( ac == 3 )
	  {
		oname = btt::genbttname(av[2]);
	  }else if( ac > 3 )
	  {
		oname = av[3];
	  }else
	  {
		std::cout<<"Internal error 10\n";
		return 10;
	  }
	  e = btt::make(av[2],oname);
	}

	if( string(av[1]) == "extr" )
	  e = btt::extr(av[2]);

	return e;
}


namespace btt{

// "aaa", "aaa.", "aaa.a", "aaa.bbb.", "aaa.bb.cc" 
string genbttname(string s)
{
	string r;
	string::size_type i;

	if( s[s.size()-1] == '.' )
		return s+"btt";

	if( (i=s.find(".")) == string::npos )
		return s+".btt";

	if( s.find(".",i+1) != string::npos )
		return s+".btt";

	r = s.substr(0,i) + ".btt";
	return r;
}


string encode(unsigned int x);

int make(string from, string to)
{
	std::ifstream in(from.c_str(),std::ios::binary);
	if( !in )
	{
	  std::cout<<"Error opening <"<<from<<">\n";
	  return 3;
	}

	std::ofstream out(to.c_str());
	if( !out )
	{
	  std::cout<<"Error opening <"<<to<<">\n";
	  return 4;
	}

	out<<from<<'\n';

	int col = 0;
	while(1)
	{
		if( !in ) break;
		char a('\0');
		in.get(a);
		if( !in ) break;
		unsigned int b = (unsigned char)a; 
		b<<=8;
		in.get(a);

		if( !in )
		{
		 out<<"~";
		 a='\0';
		}

		b |= (unsigned char)a;
		string ss = encode(b);
		out<< ss;
		if( (col+=ss.size()) > 50 ){
		 col=0;
		 out<<'\n';
		}
	}

	out<<'\n';
	return 0;
}


int extr2(std::ifstream&, std::ofstream&);

int extr(string from)
{
	std::ifstream in(from.c_str());
	if( !in )
	{
	  std::cout<<"Error opening <"<<from<<">\n";
	  return 3;
	}
	
	string to;
	std::getline(in,to);
	while( to.size() && to[to.size()-1] == '\r' )
		to = to.erase(to.size()-1,1);

	{ //check if the file present
		std::ifstream xx(to.c_str());
		if( xx )
		{
		  std::cout<<"File exists <"<<to<<">\n";
		  return 5;
		}
	}

	std::ofstream out(to.c_str(),std::ios::binary);
	if( !out )
	{
	  std::cout<<"Error opening <"<<to<<">\n";
	  return 4;
	}

	return extr2(in,out);
}

const unsigned char off = 33;
const unsigned char base = 45;

string encode(unsigned int x)
{
	string r;
	char c1, c2, c3;

	int a = x%base;
	c1 = a+off;
	x /= base;
	a = x%base;
	c2 = a+off;
	x /= base;
	a = x%base;
	c3 = a+off;

	if( c2 == off && c3 == off )
	{
		c1 += base;
		return r=c1;
	}

	r = c1;
	if( c3 == off )
	{
		c2 += base;
		r += c2;
		return r;
	}

	r += c2;
	r += c3;

	return r;
}


void wrt(std::ofstream& o, string s, int bytes);

// 0 - EOF, 1 - last byte, 2 - 2bytes, 3 - error
int getchunk(std::ifstream& in, string* s);

int extr2(std::ifstream& in, std::ofstream& out)
{
	string ch;
	while(1)
	{
		int e = getchunk(in,&ch);
		if( e == 0 ) break;
		if( e == 3 ) return 7;
		wrt(out,ch,e);
		if( e == 1 ) break;
	}
	return 0;
}

// 0 - EOF, 1 - last byte, 2 - normal, 3 - error
int getchunk(std::ifstream& in, string* s)
{
	if( !in ) return 0;
	char a('\0');
	while( !!in && a<off ) in.get(a);

	if( !in ) return 0;

	int end = 2;
	if( a == '~' )
	{
		in.get(a);
		end = 1;
	}

	if( a >= off + base )
	{
		*s = a-base;
		return end;
	}
	*s = a;
	in.get(a);
	if( a > off + base )
	{
		*s += a-base;
		return end;
	}
	*s += a;

	if( !in )
	{
		std::cout<<"Btt file corrupted\n";
		return 3;
	}

	in.get(a);
	*s += a;

	return end;
}


void wrt(std::ofstream& o, string s, int bytes)
{
	int x = 0;
	for( int i=s.size()-1; i>=0; i-- )
	{
		x *= base;
		x += s[i] - off;
	}

	char a = (char)x;
	x>>=8;
	char b = (char)x;
	o.put(b);
	if( bytes != 1 ) o.put(a);
}

} // namespace

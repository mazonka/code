#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <io.h>
#include <fcntl.h>

using namespace std;

char *inname;
long length;

long findnextgood(ifstream *in, long off)
{
	static long step0=1;
	long step=step0/2+1, off0=off;
	bool back=false;
reopen:
	delete in;
	in = new ifstream(inname,ios::binary);

	if( !*in ){
		cerr<<"Cannot reopen "<<inname<<", try again? (y/n) ";
		char y;
		cin>>y;
		if( y=='y' || y=='Y' ) goto reopen;
		exit(6);
	}

retry:
	in->seekg(off);

	char a;
	in->get(a);

//if( off >1000 && off<2000 ) {}else // TEST !!!!!

	if( !!*in )
	{
		if( !back ) cout<<"backing "<<off<<' ';
        	back = true;
		off--;
		if( off <= off0 ){ cout<<" next "<<off+1<<'\n'; return off+1; }
		//cout<<'.';
		goto retry;
	}

	if( back )
	{ 
		delete in;
		in = new ifstream(inname,ios::binary);
		cout<<" ok "<<off+1<<'\n'; 
		return off+1; 
	}

	step0 = step;
	off += (step*=2);

	if( off >= length ) return length;

	cout<<"trying "<<off<<'\n';

	goto reopen;

}

int main(int ac, char *av[]){

	if( ac < 3 ){
		cerr<<"Use: FILE_from FILE_to [off-start [off-end]]\n";
		return 1;
	}

	long offset = 0;

	if( ac > 3 )
	{
		offset = atol(av[3]);
		cout<<"Initial offset "<<offset<<'\n';
	}


	int fh;

	if( (fh=_open(av[1],_O_RDONLY)) == -1 ){
		cerr<<"Cannot open "<<av[1]<<'\n';
		return 2;
	}

	length = _filelength(fh);
	_close(fh);

	cout<<"Size "<<length<<" bytes\n";

	if( ac > 4 && length  >= atol(av[4]) )
	{
		length = atol(av[4]);
		cout<<"Final offset "<<length<<'\n';
	}

	ifstream * in = new ifstream(av[1],ios::binary);
	inname = av[1];
	ofstream out(av[2],ios::binary);

	if( !*in ){
		cerr<<"Internal error 3\n";
		delete in;
		return 3;
	}
	if( !out ){
		cerr<<"Cannot open "<<av[2]<<'\n';
		delete in;
		return 4;
	}

	in->seekg(offset);

repeat:
	char a;
	while( 1 ){
		in->get(a);

//if( offset >1000 && offset<2000 ) break; // TEST !!!!!

		if( !*in ) break;
		out<<a;

		if( offset == length ){
			delete in;
			return 0;
		}

		offset++;
		if( offset%10000 == 0 )
			cout<<"read "<<offset<<"\r";
	}
	
	if( offset > length ){
		cerr<<"Internal error 5\n";
		delete in;
		return 5;
	}
	if( offset == length ){
		delete in;
		return 0;
	}

	cout<<"Bad offset "<<offset<<"\n";

	out<<'\0';
	offset++;

	long newoff = findnextgood(in,offset);

	while( offset < newoff )
	{
		out<<'\0';
		offset++;
	}

	// seek is after if check, because there can be
	// next offset byte bad as well
	in->seekg(offset);

	goto repeat;
}

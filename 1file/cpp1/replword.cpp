// use: replword forward file-repl file-in file-out

// file-repl
// Vasil | Vasia

#include "ccrun.h"
#include "ccrut.h"

inline void eatSpaces(string * ps)
{
    string & s = *ps;
    while ( !s.empty() && std::isspace(s[0]) ) s = s.substr(1);
    while ( !s.empty() && std::isspace(s[s.size() - 1]) )
        s = s.substr(0, s.size() - 1);
}

void cmain()
{
	if( args.size() != 4 )
	{
		cout<<"Usage: replword swap/back file-repl file-in file-out\n";
		return;
	}

	string a0 = args[0];

	bool forward = (a0=="swap");
	if( a0=="back" ) forward = false;
	else if( !forward )
	{
		cout<<"Use swap/back\n";
		return;
	}

	ifstream re(args[1]);
	if( !re ) throw "cannot open ["+args[1]+"]";

	///ifstream in(args[2]);
	///if( !in ) throw "cannot open ["+args[2]+"]";

	///ofstream on(args[3]);
	///if( !on ) throw "cannot open ["+args[3]+"]";

	// load reps
	using pss = std::pair<string,string>;
	ol::ivec<pss> reps;
	while(1)
	{
		string s,t;
		std::getline(re,s,'|');
		eatSpaces(&s);
		if( s.empty() ) break;
		std::getline(re,t);
		eatSpaces(&t);
		cout<<"["<<s<<"] -> ["<<t<<"]\n";
		reps += forward ? pss(s,t) : pss(t,s);
	}	

	cout<<"loaded "<<reps.size()<<" pairs\n";

	string file = ol::file2str(args[2]);

	for( pss kv : reps )
	{
		string k = kv.first, v = kv.second;
		if( string::npos != v.find(k) ) 
			throw "bad pair ["+k+"]->["+v+"]";

		ol::replaceAll(file,k,v);
	}

	ofstream(args[3],std::ios::binary)<<file;
}

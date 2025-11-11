#include "ccrun.h"
#include "ccrut.h"

void cmain()
{
    auto dir = fsut::readdir();
    cout << "dir size = " << dir.size() << "\n";

    std::map<string, ol::ivec<fs::path>> base2path;
    for ( auto fe : dir.files() )
    {
        fs::path fp(fe.first);
        string ext = fp.extension().string();
        if ( ext != ".mp4" ) continue;

        //string basename = fp.stem().stem().stem().string();
	string basename = fp.string();
	auto sz = basename.size();
	if( sz < 5 ) continue;
	basename = basename.substr(0,sz-4);
	
        if ( basename[basename.size() - 1] == 'x' ) continue;
        cout << basename << '\n';
        base2path[basename] += fp;
    }

	for( auto me : base2path )
	{
		//if( me.second.size() != 1 ) continue;
		string b = me.first;
		string n1 = b+".mp4";
		string n2 = b+"x.mp4";
		cout<<n1<<" --> "<<n2<<'\n';
		sys("ff -n -i "+n1+" "+n2);
	}

}

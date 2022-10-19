
#include "crun.h"

void cmain()
{
    fs::path rp = "..";
    auto gf = rp / "gf.exe";
    if ( !fs::exists(gf) ) throw "no gf";

    sys("make");

    auto cdir = fs::directory_iterator(".");
    for ( auto const & de : cdir )
    {
        string nm = de.path().filename().string();
        auto nmsz = nm.size();
        if ( nmsz < 4 || nm.substr(nmsz - 4) != ".cpp" ) continue;
	if( nm[0] != 't' ) continue; // exclude self-run
        //cout << nm << '\n';
	sys("crun "+nm+" 2>&1 | grep \"OK\\|FAIL\\|error\"");
    }

    sys("make c");
}

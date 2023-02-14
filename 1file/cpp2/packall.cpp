#include "ccrun.h"
#include "ccrut.h"

string ext;

string gf = "c:\\apg\\app\\bin\\gf.exe pack ";

void cmain()
{
    auto ents = fsut::readdir();

    for ( auto e : ents.dirs().names() )
        sys(gf + e);

    if ( args.size() > 0 ) ext = args[0];

    if ( !ext.empty() )
        for ( auto e : ents.files().names() )
            if ( ol::endsWith(e, ext) )
                sys(gf + e);
}

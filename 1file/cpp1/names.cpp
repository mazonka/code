#include "ccrun.h"
#include "ccrut.h"


void cmain()
{
    auto ents = fsut::readdir(true);

    for ( auto x : ents.files().names() )
    {
        cout << "[" << x << "]";
        string f = ol::file2str(x);
        cout << ' ' << f.size() << '\n';
    }
}

#include "crun.h"

void cmain()
{
    ofstream("test1.tmp") << "";
    cout<<(fs::path("..") /"bzc.exe") << '\n';
    system(fs::path("../bzc.exe").string().c_str());
}

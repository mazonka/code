
#include "samehf.h"
#include "copyfile.h"

bool inclDot = false;

void olmain(vector<string> & av)
{
    if ( av.size() < 2 )
    {
        std::cout << "samh, Oleg Mazonka, 2019, v0.1\n";
        std::cout << "Usage: longname, cutname\n";
///        std::cout << "Use @ (e.g. sameh@) to include '.' files and dirs\n";
///        std::cout << "sameh - find same files in the current dir\n";
///        std::cout << "copy D1 D2 D3 - copies files D1 to D2 moving from D3\n";
///        std::cout << "nocopy - checks files D1 by moving from D3 to D2\n";
///        std::cout << "nocopy - same as copy but does not copy files\n";
        std::cout << "longname - prints long file names\n";
        std::cout << "cutname ext pos size - cut long filenames\n";
        return;
    }
}

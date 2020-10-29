// implementation for MS Visual C, Windows

#include <direct.h>
#include <io.h>
#include <fcntl.h>

#include "readdir.h"

std::map<std::string, std::pair<unsigned long, long> >
readdir()
{

    std::map<std::string, std::pair<unsigned long, long> > r;
    _wfinddata_t f;
    auto handle = _wfindfirst(L"*", &f);

    if ( (handle) != -1L )
        do
        {
            // f.name f.size, (f.attrib & _A_SUBDIR)

            std::pair<unsigned long, long> atr;

            atr.first = (unsigned long)f.time_write;

            if ( !(f.attrib & _A_SUBDIR)  )
                atr.second =  f.size;
            else
                atr.second =  -1L;

            std::wstring w(f.name);
            std::string name(w.begin(),w.end());

            if ( name != "." && name != ".." )
                r[name] = atr;

        }
        while ( _wfindnext(handle, &f) != -1 );

    _findclose(handle);

    return r;
}

void setmode2binary(FILE* f) { if (_setmode(_fileno(f), _O_BINARY)) {} }

namespace dir
{

bool setd(std::string s) { return !_chdir(s.c_str()); }
std::string getd()
{
    int size = 100;
    char * p = new char[size];
    char * r = _getcwd(p, size);
    while ( !r )
    {
        delete[] p;
        if ( size > 1000000L ) return "";
        size *= 2;
        p = new char[size];
        r = _getcwd(p, size);
    }

    std::string rr(r);
    delete[] p;
    return rr;

}

bool make(std::string s)
{
    return !_mkdir(s.c_str());
}

bool remove(std::string s)
{
    return !_rmdir(s.c_str());
}

bool rename(std::string old, std::string n)
{
    return !::rename(old.c_str(), n.c_str());
}

} //dir


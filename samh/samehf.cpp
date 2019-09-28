// this is a Windows program (can be easily ported to Unix by Hasq dev)
// to find same files

#include "samehf.h"

using namespace ol;

sam::mfu sam::getListOfFiles(os::Path p, bool dot)
{
    static ull sz = 0;

    string pstr = p.str();
    string prefix = pstr + '/';
    if (pstr.empty()) never(1);
    if (pstr == ".") prefix = "";

    mfu r;
    os::Dir d = os::FileSys::readDirEx(p, true, true);

    sz += d.files.size();
    cout << sz << '\r' << std::flush;

    for ( auto f : d.files )
    {
        if ( !dot && f.first[0] == '.' ) continue;
        string n = prefix + f.first;
        r[File {prefix, f.first, os::FileSys::mtime(n), ull(f.second)}] = f.second;
    }
    for ( auto f : d.dirs )
    {
        if ( !dot && f[0] == '.' ) continue;
        auto n = getListOfFiles(prefix + f, dot);
        r.insert(n.begin(), n.end());
    }

    return r;
}

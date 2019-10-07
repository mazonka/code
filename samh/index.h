
#include <map>

#include "samf.h"

struct Hfile
{
    sam::File file;
    QfHash hash;

    string str() const;

    bool operator<(const Hfile & x) const
    {
        if ( file < x.file ) return true;
        if ( x.file < file ) return false;
        return hash < x.hash;
    }

    Hfile() = default;
    Hfile(std::pair<sam::File, QfHash> a): file(a.first), hash(a.second) {}
    Hfile(sam::File f, QfHash h): file(f), hash(h) {}
    Hfile(sam::File f): file(f), hash() {}
};

class IndexFile : public std::map<sam::File, QfHash>
{
        string filename;

    public:
        IndexFile(string fn);
        IndexFile() {}

        void save(string fn) const;
        void save() const { save(filename); }
};


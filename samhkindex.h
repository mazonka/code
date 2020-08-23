
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

    enum MakeHash_t { MakeHashQF = 3, MakeHashQ = 1, MakeHashF = 2, NoHash = 0 };
    Hfile(sam::File f, MakeHash_t);

    Hfile(std::pair<const sam::File, QfHash> a): file(a.first), hash(a.second) {}
};

class IndexFile : public std::map<sam::File, QfHash>
{
        string filename;

    public:
        IndexFile(string fn, bool top = false);
        IndexFile() {}

        void save(string fn) const;
        void save() const { save(filename); }
};



#include <map>

#include "sam.h"

struct QfHash
{
    string q; // 100-hash
    string f; // full hash

    string str() const;

    bool operator<(const QfHash & x) const
    {
        if ( q < x.q ) return true;
        if ( x.q < q ) return false;
        return f < x.f;
    }
};

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


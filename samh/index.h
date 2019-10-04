
#include <map>
#include <set>

#include "samehf.h"

struct Hfile
{
    sam::File file;
    string qhash; // 100-hash
    string fhash; // full hash

    string str() const;

    bool operator<(const Hfile & x) const
    {
        if ( file < x.file ) return true;
        if ( x.file < file ) return false;
        if ( qhash < x.qhash ) return true;
        if ( x.qhash < qhash ) return false;
        return fhash < x.fhash;
    }

};
// levels of trust
// size mtime dname fname qhash fhash
// size - a must
// change mtime - access - maybe
// change dname - move - maybe
// change fname - rename - maybe
// change qhash - modification - maybe
// fhash - proof
// 1000?? 1001?? 1010?? 1100?? 1011?? 1101?? 1110?? 1111??
//

class IndexFile : public std::set<Hfile>
{
        string filename;

        void save(string fn) const;

    public:
        IndexFile(string fn);

        void save() const { save(filename); }
};


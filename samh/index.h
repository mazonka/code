
#include "samehf.h"

struct Hfile
{
    sam::File file;
    string qhash; // 100-hash
    string fhash; // full hash
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

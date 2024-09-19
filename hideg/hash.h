#include <string>

using std::string;

namespace ha
{
string hashHex(const string & s);
string toHex(const string & s);
string toBin(const string & s);

struct StreamHashHex
{
    void * ptr;

    StreamHashHex();
    ~StreamHashHex();
    void add(const void * data, size_t sz);
    string hex() const;

    void operator=(const StreamHashHex &) = delete;
    StreamHashHex(const StreamHashHex &) = delete;
};

} // ha

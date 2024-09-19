#include <cstring>
#include "sha256.h"

#include "hash.h"

using std::memcpy;

namespace ha
{

void calcSha256Hash(const char * in, int inlen, char * out);

const size_t HashBitSz = 256;
const size_t HashBteSz = HashBitSz / 8;
const size_t HashHexSz = HashBteSz * 2;

inline int st2i(size_t x) { return static_cast<int>( x ); }

/* old sha256
string hashHex(const string & s)
{
    char b[HashHexSz];
    calcSha256Hash(s.c_str(), st2i(s.size()), b);
    return string(b, HashHexSz);
}
*/

string hashHex(const string & s)
{
    SHA256 sha256;
    return sha256(s);
}

void toHex(const char * in, int inlen, char * out);
void toBin(const char * in, int inlen, char * out);
string toHex(const string & s);
string toBin(const string & s);

StreamHashHex::StreamHashHex()
{
    ptr = new SHA256;
}

StreamHashHex::~StreamHashHex()
{
    SHA256 * p_sha256 = static_cast<SHA256 *>(ptr);
    delete p_sha256;
}

void StreamHashHex::add(const void * data, size_t sz)
{
    SHA256 * p_sha256 = static_cast<SHA256 *>(ptr);
    p_sha256->add(data, sz);
}

string StreamHashHex::hex() const
{
    SHA256 * p_sha256 = static_cast<SHA256 *>(ptr);
    return p_sha256->getHash();
}

} // hash

/* old sha256
void ha::calcSha256Hash(const char * in, int inlen, char * out)
{
    char buf[SHA256_DIGEST_STRING_LENGTH];

    ///SHA256_Data((const u_int8_t*)in, inlen, buf);
    SHA256_Data((const uint8_t*)in, inlen, buf);
    memcpy(out, buf, SHA256_DIGEST_STRING_LENGTH - 1);
}
*/

void ha::toHex(const char * in, int inlen, char * out)
{
    const char * hexDigits = "0123456789abcdef";
    int index = 0;

    if (in && inlen > 0 && out)
    {
        for (int i = 0; i < inlen; i++)
        {
            out[index++] = hexDigits[(in[i] >> 4) & 0x0F];   // equal to "in[i]/16"
            out[index++] = hexDigits[in[i] & 0x0F];          // equal to "in[i]%16"
        }
    }
}

void ha::toBin(const char * in, int inlen, char * out)
{
    int index = 0;

    if (in && inlen > 0 && out)
    {
        for (int i = 0; i < inlen; i++)
        {
            char a = in[i];
            if ( a >= '0' && a <= '9' ) a -= '0';
            else if ( a >= 'a' && a <= 'f' ) a -= 'a' - char(10);

            if ( i % 2 )
            {
                out[index] <<= 4;
                out[index++] |= a;
            }
            else
                out[index] = a;
        }
    }
}

string ha::toHex(const string & s)
{
    size_t sz = s.size();
    char * buf = new char[sz * 2];
    toHex(s.c_str(), st2i(sz), buf);
    string r(buf, sz * 2);
    delete [] buf;
    return r;
}

string ha::toBin(const string & s)
{
    size_t sz = s.size();
    char * buf = new char[sz];
    toBin(s.c_str(), st2i(sz), buf);
    string r(buf, sz / 2);
    delete [] buf;
    return r;
}


///#include "sha2.cpp"

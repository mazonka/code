
// lifter lifts uint type to double its size

// Oleg Mazonka, 2023

#pragma once

#include <string>
using std::string;

template <class U>
struct lifter
{
    U lo, hi;
};

template <class U>
lifter<U> operator+(const lifter<U> & a, const lifter<U> & b)
{
    U lo = a.lo + b.lo;
    U hi = a.hi + b.hi;
    U carry = U {uint8_t(lo < a.lo)};
    return {lo, U(hi + carry)};
}

template <class U>
lifter<U> & operator+=(lifter<U> & a, const lifter<U> & b)
{
    U x = a.lo + b.lo;
    a.hi += b.hi + U {uint8_t(x < a.lo)};
    a.lo = x;
    return a;
}

template <class U>
inline bool operator!(const lifter<U> & a) { return !(a.lo | a.hi); }

template <class U>
inline bool operator==(const lifter<U> & a, const lifter<U> & b)
{
    return ((a.hi == b.hi) && (a.lo == b.lo));
}

template <class U>
inline bool operator<(const lifter<U> & a, const lifter<U> & b)
{
    if (a.hi == b.hi) return (a.lo < b.lo);
    return (a.hi < b.hi);
}

template <class U>
inline bool operator>(const lifter<U> & a, const lifter<U> & b)
{
    if (a.hi == b.hi) return (a.lo > b.lo);
    return (a.hi > b.hi);
}

template <class U>
inline bool operator>=(const lifter<U> & a, const lifter<U> & b)
{
    return ((a > b) | (a == b));
}

template <class U>
inline bool operator<=(const lifter<U> & a, const lifter<U> & b)
{
    return ((a < b) | (a == b));
}


template <class U>
int bits(const lifter<U> & a)
{
    int cnt = 0;
    if (!!a.hi)
    {
        cnt = sizeof(U) * 8;
        U up = a.hi;
        while (!!up) { up >>= 1; cnt++; }
    }
    else
    {
        U lo = a.lo;
        while (!!lo) { lo >>= 1; cnt++; }
    }
    return cnt;
}

template <class U>
lifter<U> operator<<(const lifter<U> & a, int b)
{
    lifter<U> U0 {U{0}};
    constexpr int szUt = sizeof(U) * 8;
    constexpr int szU2 = szUt * 2;

    if (b >= szU2) return U0;
    else if (b == szUt) return lifter<U> {U{0}, a.lo};
    else if (b == 0) return a;
    else if (b < szUt)
    {
        U x1 = a.lo << b;
        U x2 = a.hi << b;
        U x3 = a.lo >> (szUt - b);
        //return lifter<U> {a.lo << b, (a.hi << b) + (a.lo >> (szU - b))};
        U x4 = x2 + x3;
        return lifter<U> {x1, x4};
    }
    else if ((szU2 > b) && (b > szUt))
    {
        U x = a.lo << (b - szUt);
        return lifter<U> {U{0}, x};
    }

    return U0;
}

template <class U>
lifter<U> operator>>(const lifter<U> & a, int b)
{
    lifter<U> U0 {U{0}};
    constexpr int szUt = sizeof(U) * 8;
    constexpr int szU2 = szUt * 2;

    if (b >= szU2) return U0;
    else if (b == szUt) return lifter<U> {a.hi, U{0}};
    else if (b == 0) return a;
    else if (b < szUt)
    {
        //return lifter<U>(UPPER >> shift, (UPPER << (64 - shift)) + (LOWER >> shift));
        U hi = a.hi >> b;
        U l1 = a.hi << (szUt - b);
        U l2 = a.lo >> b;
        U l3 = l1 + l2;
        return lifter<U> { l3, hi };
    }
    else if ((szU2 > b) && (b > szUt))
    {
        U lo = a.hi >> (b - szUt);
        return lifter<U> {lo, U{0}};
    }

    return U0;
}

template <class U>
inline lifter<U> & operator<<=(lifter<U> & a, int b)
{
    a = a << b;
    return a;
}

template <class U>
inline lifter<U> & operator>>=(lifter<U> & a, int b)
{
    a = a >> b;
    return a;
}

template <class U>
inline lifter<U> operator&(const lifter<U> & a, const lifter<U> & b)
{
    return lifter<U> {U(a.lo & b.lo), U(a.hi & b.hi)};
}

template <class U>
inline lifter<U> operator/(const lifter<U> & a, const lifter<U> & b)
{
    return divmod(a, b).first;
}

template <class U>
inline lifter<U> & operator/=(lifter<U> & a, const lifter<U> & b)
{
    a = a / b;
    return a;
}

template <class U>
inline lifter<U> operator%(const lifter<U> & a, const lifter<U> & b)
{
    return divmod(a, b).second;
}

template <class U>
inline lifter<U> & operator%=(lifter<U> & a, const lifter<U> & b)
{
    a = a % b;
    return a;
}

template <class U>
inline lifter<U> & operator++(lifter<U> & a) { return a += {U{1}}; }

template <class U>
inline lifter<U> operator++(lifter<U> & a, int)
{
    lifter<U> t(a);
    ++a;
    return t;
}

template <class U>
inline lifter<U> & operator--(lifter<U> & a) { return a -= {U{1}}; }

template <class U>
inline lifter<U> operator--(lifter<U> & a, int)
{
    lifter<U> t(a);
    --a;
    return t;
}

template <class U>
inline lifter<U> operator+(const lifter<U> & a) { return a; }

template <class U>
inline lifter<U> operator-(const lifter<U> & a) { return ~a + lifter<U> {U{1}}; }


template <class U>
std::pair <lifter<U>, lifter<U>> divmod(const lifter<U> & a, const lifter<U> & b)
{
    using pll = std::pair <lifter<U>, lifter<U>>;

    lifter<U> U0 {U{0}}, U1 {U{1}};
    pll qr (U0, U0);

    if (b == U0) return qr;
    else if (b == U1) return pll ({a}, U0);
    else if (a == b) return pll (U1, U0);
    else if ((a == U0) || (a < b)) return pll (U0, a);

    for (int x = bits(a); x > 0; x--)
    {
        qr.first  <<= 1;
        qr.second <<= 1;

        if ( !!((a >> (x - 1)) & U1) ) ++qr.second;

        if (qr.second >= b)
        {
            qr.second -= b;
            ++qr.first;
        }
    }
    return qr;
}

inline int toint(const uint64_t & a) { return int(a); }
inline int toint(const uint32_t & a) { return int(a); }
inline int toint(const uint16_t & a) { return int(a); }
inline int toint(const uint8_t & a) { return int(a); }

template <class U>
inline int toint(const lifter<U> & a) { return toint(a.lo); }

template <class U>
string str(const lifter<U> & a, uint8_t base, int len = 0)
{
    if ((base < 2) || (base > 16)) return "";

    string out;
    if (!a) return "0";

    std::pair <lifter<U>, lifter<U>> qr({a}, {U{0}});
    do
    {
        qr = divmod(qr.first, lifter<U> {base} );
        auto idx = toint(qr.second);
        out = "0123456789abcdef"[idx] + out;
    }
    while (!!qr.first);

    if (out.size() < len)
    {
        out = string(len - out.size(), '0') + out;
    }

    return out;
}

template <class U>
std::ostream & operator<<(std::ostream & os, const lifter<U> & a)
{
    if (os.flags() & os.oct) os << str<U>(a, 8);
    else if (os.flags() & os.dec) os << str(a, 10);
    else if (os.flags() & os.hex) os << str(a, 16);
    return os;
}

template <class U>
lifter<U> operator*(const lifter<U> & a, const lifter<U> & b)
{
    lifter<U> U0 {U{0}};
    constexpr int szUt = sizeof(U) * 8;
    constexpr int szU2 = szUt * 2;
    constexpr int szUh = szUt / 2;
    U ff = ~U {0};
    U of = ff >> szUh;

    U top[4] = {U(a.hi >> szUh), U(a.hi & of), U(a.lo >> szUh), U(a.lo & of)};
    U bot[4] = {U(b.hi >> szUh), U(b.hi & of), U(b.lo >> szUh), U(b.lo & of)};
    U prod[4][4];

    // multiply each component of the values
    for (int y = 3; y > -1; y--)
        for (int x = 3; x > -1; x--)
            prod[3 - x][y] = top[x] * bot[y];

    // first row
    U fourt32 = (prod[0][3] & of);
    U third32 = (prod[0][2] & of) + (prod[0][3] >> szUh);
    U secon32 = (prod[0][1] & of) + (prod[0][2] >> szUh);
    U first32 = (prod[0][0] & of) + (prod[0][1] >> szUh);

    // second row
    third32 += (prod[1][3] & of);
    secon32 += (prod[1][2] & of) + (prod[1][3] >> szUh);
    first32 += (prod[1][1] & of) + (prod[1][2] >> szUh);

    // third row
    secon32 += (prod[2][3] & of);
    first32 += (prod[2][2] & of) + (prod[2][3] >> szUh);

    // fourth row
    first32 += (prod[3][3] & of);

    // move carry to next digit
    third32 += fourt32 >> szUh;
    secon32 += third32 >> szUh;
    first32 += secon32 >> szUh;

    // remove carry from current digit
    fourt32 &= of;
    third32 &= of;
    secon32 &= of;
    first32 &= of;

    // combine components
    U lo = (third32 << szUh) | fourt32;
    U hi = (first32 << szUh) | secon32;
    return {lo, hi};
}

template <class U>
inline lifter<U> operator-(const lifter<U> & a, const lifter<U> & b)
{
    U lo = a.lo - b.lo;
    U hi = a.hi - b.hi - U {uint8_t(lo > a.lo)};
    return lifter<U> {lo, hi};
}

template <class U>
inline lifter<U> & operator-=(lifter<U> & a, const lifter<U> & b)
{
    a = a - b;
    return a;
}

template <class U>
inline bool operator!=(const lifter<U> & a, const lifter<U> & b)
{
    return ((a.hi != b.hi) | (a.lo != b.lo));
}


template <class U>
inline lifter<U> operator|(const lifter<U> & a, const lifter<U> & b)
{
    return lifter<U> {U(a.lo | b.lo), U(a.hi | b.hi)};
}

template <class U>
inline lifter<U> & operator|=(lifter<U> & a, const lifter<U> & b)
{
    a.hi |= b.hi;
    a.lo |= b.lo;
    return a;
}

template <class U>
inline lifter<U> operator^(const lifter<U> & a, const lifter<U> & b)
{
    return lifter<U> {U(a.lo ^ b.lo), U(a.hi ^ b.hi)};
}

template <class U>
inline lifter<U> & operator^=(lifter<U> & a, const lifter<U> & b)
{
    a.hi ^= b.hi;
    a.lo ^= b.lo;
    return a;
}

template <class U>
inline lifter<U> operator~(const lifter<U> & a)
{
    return lifter<U> {U(~a.lo), U(~a.hi)};
}

template <class U>
inline lifter<U> & operator&=(lifter<U> & a, const lifter<U> & b)
{
    a.hi &= b.hi;
    a.lo &= b.lo;
    return a;
}

template <class U>
inline lifter<U> operator*=(lifter<U> & a, const lifter<U> & b)
{
    a = a * b;
    return a;
}


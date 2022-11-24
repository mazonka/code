#include <string>

constexpr int NDIG = 3;

using ull = unsigned long long;
constexpr ull by10(ull x){ return x? 10*by10(x-1) : 1; } 
constexpr ull NMAX = by10(NDIG);

using std::string;

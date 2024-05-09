
// Example to use lifter

#include <iostream>

#include "lifter.h"


using std::cout;

void testops()
{
    using lint = lifter<lifter<lifter<lifter<uint64_t>>>>;
    cout << "sizeof in bits = " << sizeof(lint) * 8 << '\n';

    // Arithmetic operators
    lint a { 10 }, b { 5 };
    cout << "Arithmetic Operators:" << '\t';
    cout << "a + b = " << a + b << '\t';
    cout << "a - b = " << a - b << '\n';
    cout << "a * b = " << a * b << '\t';
    cout << "a / b = " << a / b << '\t';
    cout << "a % b = " << a % b << '\n';

    // Relational operators
    cout << "Relational Operators:" << '\t';
    cout << "a == b is " << (a == b) << '\t';
    cout << "a != b is " << (a != b) << '\n';
    cout << "a > b is " << (a > b) << '\t';
    cout << "a < b is " << (a < b) << '\t';
    cout << "a >= b is " << (a >= b) << '\t';
    cout << "a <= b is " << (a <= b) << '\n';

    // Logical operators
    lint x { 1 };
    cout << "Logical Operators:" << '\t';
    cout << "!x is " << !x << '\n';

    // Bit-wise operators
    lint num1 { 5 }, num2 { 3 };
    cout << "Bit-wise Operators:" << '\t';
    cout << "num1 & num2 = " << (num1 & num2) << '\t'; // Bitwise AND
    cout << "num1 | num2 = " << (num1 | num2) << '\n'; // Bitwise OR
    cout << "num1 ^ num2 = " << (num1 ^ num2) << '\t'; // Bitwise XOR
    cout << "-~num1 = " << (-~num1) << '\n';             // Bitwise NOT
    cout << "num1 &= num2 = " << (num1 &= num2) << '\t'; // Bitwise AND
    cout << "num1 |= num2 = " << (num1 |= num2) << '\t'; // Bitwise OR
    cout << "num1 ^= num2 = " << (num1 ^= num2) << '\n'; // Bitwise XOR

    // Shift operators
    lint num3 { 8 };
    cout << "Shift Operators:" << '\t';
    cout << "num3 << 2 = " << (num3 << 2) << '\t'; // Left shift by 2
    cout << "num3 >> 1 = " << (num3 >> 1) << '\n'; // Right shift by 1

    // Assignment operators
    lint c { 15 };
    cout << "Assignment Operators: (c=15)" << '\t';
    c += c;
    cout << "c += c is " << c << '\t';
    c -= c - b;
    cout << "c -= 3 is " << c << '\n';
    c *= c;
    cout << "c *= c is " << c << '\t';
    c /= c;
    cout << "c /= c is " << c << '\t';
    c %= b;
    cout << "c %= 3 is " << c << '\n';

    c %= c;
    cout << str(--c, 16) << '\n';
}


int main()
{
    str(lifter<uint32_t> {50}, 10);
    lifter<uint64_t> c {1};
    lifter<uint8_t> a {1};
    auto b = a + a;
    cout << (b + b);
    cout << ' ' << (b * b) << '\n';

    testops();
}



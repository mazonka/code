// mazonka constant

#include <iostream>

int main()
{
    int a = 1, b = 1;
    for ( int i = 0; i < 60; i++ )
    {
        std::cout << a;
        int x = a + b;
        a = b;
        b = x % 10;
    }
}

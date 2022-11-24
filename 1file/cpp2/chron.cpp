#include <iostream>
#include <string>

#include "chron.h"


using namespace std;

int main()
{
    auto s = chron::now();
    cout << "Enter something : ";
    string a; cin >> a;
    auto e = chron::now();
    cout << "millisecs: " << (e - s) << '\n';
}


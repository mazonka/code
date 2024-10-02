

#include <iostream>
#include <string>

using std::string;

using std::cout;
using std::cin;

void cmd()
{
    while (1)
    {
        string s;
        cin >> s;
        cout << "FDBACK: " << s << '\n';
        if ( s == "exit" ) break;
    }
}

int main()
{
    cmd();
}

// this prigram reads a file in the following format
// spanish lines followed by an empty line, then english lines
// the number of spanish and english lines must be the same
// it outputs 'at' format to stdout

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

int main(int ac, char * av[])
try
{
    if ( ac < 2 )
    {
        cout << "Usage: filename\n";
        return 0;
    }

    void f(string);
    f(av[1]);

}
catch (...)
{
    cout << "error";
    return 1;
}

string getLine(istream & in)
{
    string line;
    while (in)
    {
        getline(in, line);
        if ( !line.empty() ) return line;
    }

    return "";
}

void getLines(istream & in, vector<string> & v)
{
    string line;
    while (in)
    {
        getline(in, line);
        if ( line.empty() ) return;
        v.push_back(line);
    }
}

void f(string fname)
{
    ifstream in(fname);

    string line = getLine(in);

    vector<string> esp; esp.push_back(line);
    getLines(in, esp);

    line = getLine(in);
    vector<string> eng; eng.push_back(line);
    getLines(in, eng);

    if ( esp.size() != eng.size() )
    {
        cout << "Number of esp lines: " << esp.size() << '\n';
        cout << "Number of eng lines: " << eng.size() << '\n';
        throw 0;
    }

    int sz = esp.size();

    for (int i = 0; i < sz; i++ )
    {
        cout << "@\n" << esp[i] << "\n#" << eng[i] << "\n\n";
    }
}

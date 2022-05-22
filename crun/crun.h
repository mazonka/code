#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <chrono>
#include <memory>

using namespace std;
namespace fs = std::filesystem;

vector<string> args;
void cmain();

int main(int ac, const char *av[])
try
{
	for( int i=1; i<ac; i++ ) args.push_back(av[i]);
	cmain();
}

catch (int e)
{
    cout << "error (int): " << e << "\n";
    return 1;
}
catch (string e)
{
    cout << "error: " << e << "\n";
    return 1;
}
catch (const char * e)
{
    cout << "error: " << e << "\n";
    return 1;
}
catch (std::exception & e)
{
    cout << "error (std): " << e.what() << "\n";
    return 1;
}
catch (...)
{
    cout << "Unknown exception\n";
    return 1;
}


#include <iostream>
#include <string>

#include "bad.h"
#include "model.h"


using std::cout;
using std::string;

int main()
try
{
    model();
}
catch (int e)
{
    cout << "Error (line): " << e << "\n";
}
catch (const char * e)
{
    cout << "Error: " << e << "\n";
}
catch (string e)
{
    cout << "Error: " << e << "\n";
}
catch (std::exception e)
{
    cout << "Error (std): " << e.what() << "\n";
}
catch (...)
{
    cout << "exception\n";
}


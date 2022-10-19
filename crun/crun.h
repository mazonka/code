#pragma once

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
#include <system_error>
#include <tuple>

///using namespace std;
namespace fs = std::filesystem;
using std::cout;
using std::string;
using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::ostringstream;


#ifndef CRUNMAIN
std::vector<string> args;
string arg0;
void cmain();
int main(int ac, const char * av[])
try
{
    for ( int i = 1; i < ac; i++ ) args.push_back(av[i]);
    arg0 = av[0];
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
#else
extern vs args;
extern string arg0;
#endif


#ifndef for_i
#define for_i(x) for(int i=0; i<(x); ++i)
#define for_ir(x) for(int i=(x); i>=0; --i)
#define for_(i,x) for(int i=0; i<(x); ++i)
#endif

#ifndef never
#define nevers(x) (throw std::string("err at ")+(__FILE__)+":"+std::to_string(__LINE__) + " ["+x+"]")
#define never (throw std::string("err at ")+(__FILE__)+":"+std::to_string(__LINE__) + " ["+(__func__)+"]")
#endif


inline int sys(const char *s){ return system(s); }
inline int sys(string s){ return sys(s.c_str()); }
inline int sys(fs::path s){ return sys(s.string()); }


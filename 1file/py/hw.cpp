// cl hw.cpp -link C:\Python27\libs\python27.lib
#include "C:\\Python27\\include\\python.h"

int
main(int argc, char *argv[])
{
  Py_SetProgramName(argv[0]);  /* optional but recommended */
  Py_Initialize();
  PyRun_SimpleString("from time import time,ctime\n"
                     "print 'Today is',ctime(time())\n");

  void runfile();
  runfile();
  
  Py_Finalize();
  return 0;
}

#include <string>
#include <fstream>
using std::string;

void runfile()
{
    std::ifstream in("hw.py");
    string s;
    for( char c; in.get(c); s+=c );
    PyRun_SimpleString(s.c_str());
}

#include <iostream>
#include <fstream>
#include <string>

#include "copyfile.h"

#include "os_filesys.h"

const static int BUF_SIZE = 4096 * 100;

using std::ios_base;

void copyfile_low(std::string sin, std::string sof)
{
    std::ifstream in(sin, ios_base::in | ios_base::binary);
    if ( !in ) throw "Cannot open " + sin;
    std::ofstream of(sof, ios_base::out | ios_base::binary);
    if ( !of ) throw "Cannot open " + sof;

    char buf[BUF_SIZE];

    do
    {
        in.read(&buf[0], BUF_SIZE);
        of.write(&buf[0], in.gcount());
    }
    while (in.gcount() > 0);

    if ( !of ) throw "copyfile: bad of";
}

void copyfile(std::string sin, std::string sof)
{
    string tmp = sof + filetmpext;
    copyfile_low(sin, tmp);
    os::FileSys::move(tmp, sof);
}

void basename(string file, string & dname, string & fname)
{
    auto k = file.rfind('/');
    if ( k == string::npos) fname = file;
    else
    {
        fname = file.substr(k + 1);
        dname = file.substr(0, k);
    }
}

void dirForFile(string n)
{
    string dname, fname;
    basename(n, dname, fname);

    os::Path ndir(dname);
    if ( !ndir.isdir() ) os::FileSys::trymkdir(dname);
    if ( !ndir.isdir() ) throw "Cannot create [" + ndir.str() + "]";
}


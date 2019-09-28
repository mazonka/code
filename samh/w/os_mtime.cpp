// Hasq Technology Pty Ltd (C) 2013-2016

#include <direct.h>
#include <io.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <cstdio>
#include <iostream>

#include <time.h>

#include "os_mtime.h"


time_t os::FileSys::mtime(const string & s)
{
    struct _stat64 buf;
    int r = _stat64( s.c_str(), &buf );
    if (r) throw "Access denied " + s;
    return buf.st_mtime;
}


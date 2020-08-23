// Hasq Technology Pty Ltd (C) 2013-2016

#include <direct.h>
#include <io.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <cstdio>
#include <iostream>
#include <iomanip>

#include <time.h>

#include "os_filesys.h"

const char * os::getCwd(char * buf, int sz)
{
    const char * r = _getcwd(buf, sz);
    if ( r ) while ( *buf && sz-- > 0 )
        {
            if ( *buf == '\\' ) *buf = Path::SL;
            ++buf;
        }
    return r;
}

bool os::makeDir(const string & s)
{
    return !_mkdir(s.c_str());
}

std::pair<int, gl::sll> os::isDirOrFile(const string & s) // 0 no, 1 file, 2 dir
{
    struct _stat64 buf;
    int r = __stat64( s.c_str(), &buf );

    if (r && s.size() )
    {
        // test unix case ".../aaa/"
        if ( s[ s.size() - 1 ] == '/' )
        {
            string u = s.substr(0, s.size() - 1);
            std::pair<int, gl::sll> k = isDirOrFile(u);
            if ( k.first == 2 ) return k;
        }

        return std::pair<int, gl::sll>(0, 0);
    }

    gl::sll sz = gl::x2sll(buf.st_size);
    if ( buf.st_mode & _S_IFDIR ) return std::pair<int, gl::sll>(2, sz);
    if ( buf.st_mode & _S_IFREG ) return std::pair<int, gl::sll>(1, sz);

    return std::pair<int, gl::sll>(0, 0);
}


os::Dir os::FileSys::readDir(Path d)
{
    Dir dir;

    __finddata64_t f;
    intptr_t handle;

    Path filemask(d + "*");

    if ( (handle = _findfirst64(filemask.str().c_str(), &f) ) != -1L )

        do
        {

            std::string name(f.name);
            if ( name == "." || name == ".." ) continue;

            if ( !(f.attrib & _A_SUBDIR)  )
            {
                // file
                gl::sll sz =  f.size;
                dir.files.push_back(std::pair<string, gl::sll>(name, sz));
            }
            else
                dir.dirs.push_back(name);

        }
        while ( _findnext64(handle, &f) != -1 );

    _findclose(handle);

    return dir;
}

bool os::rmDir(const string & s)
{
    return !_rmdir(s.c_str());
}

bool os::rmFile(const string & s)
{
    int r = _unlink(s.c_str());
    ///if ( r == -1 ) perror(nullptr);
    return !r;
}

bool os::rename(std::string old, std::string n)
{
    return !::rename(old.c_str(), n.c_str());
}

bool os::FileSys::truncate(const string & s, gl::sll size)
{
    bool ret = false;
    int fd = _open(s.c_str(), _O_RDWR);
    //int fd = _sopen_s(s.c_str(), _O_RDWR, _SH_DENYNO);

    if ( fd != -1 )
    {
        if ( _chsize_s(fd, size) == 0 )
            ret = true;
        _close(fd);
    }
    return ret;
}

double os::FileSys::howold(const string & s)
{
    struct _stat64 buf;
    int r = _stat64( s.c_str(), &buf );
    if (r) return -1;
    return difftime(time(0), buf.st_mtime);
}


//#ifdef _O_BINARY
struct Setmode2binary
{
    Setmode2binary()
    {
        std::ios_base::sync_with_stdio(true);
        (void)_setmode( _fileno(stdout), _O_BINARY );
    }
} Setmode2binary_dummy;
//#endif

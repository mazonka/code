#pragma once

#include <map>
#include <string>
#include <filesystem>
#include <system_error>

using std::string;
namespace fs = std::filesystem;

typedef std::map<std::string, std::pair<unsigned long long, long> > msull;

template <class T>
msull readdirT()
{

    msull r;

    auto cdir = fs::directory_iterator(".");
    ///DIR* dir = opendir(".");

    ///if (!dir->is_directory()) return r;

    ///struct dirent* de;

    for ( auto const & de : cdir )
    {
        ///struct stat st;
        ///std::string nm(de->d_name);
        string nm = de.path().filename().string();

        ///if (nm == "." || nm == "..") continue;

        ///if (-1 == stat(nm.c_str(), &st)) continue;

        unsigned long long tm = de.last_write_time().time_since_epoch().count();

        long sz = -1L;
        if (!de.is_directory()) sz = (long)de.file_size();

        r[nm] = std::pair<unsigned long long, long>(tm, sz);
    }

    return r;
}

inline msull readdir() { return readdirT<void>(); }

namespace dir
{
    inline string getd() { return fs::current_path().string(); }
    inline bool setd(string s) 
    {
        std::error_code e;
        fs::current_path(s,e);
        return !e.default_error_condition();
    }
    inline void make(string s) { fs::create_directories(s); }

} // dir

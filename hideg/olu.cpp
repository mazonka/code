#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

#include "olu.h"

namespace fs = std::filesystem;
using std::cout;

bool ol::endsWith(string s, string fx)
{
    auto sz = s.size();
    auto fz = fx.size();
    return sz >= fz && 0 == s.compare(sz - fz, fz, fx);
};

bool ol::endsWith(string s, string fx, string & cut)
{
    bool r = endsWith(s, fx);
    if ( r ) cut = s.substr(0, s.size() - fx.size());
    return r;
}

string ol::file2str(const fs::path & file)
{
    const size_t MAX_FILE_SIZE = 1024u * 1024 * 1000; // 1000Mb
    std::ifstream in(file, std::ios::binary);

    if ( !in )
        return "";

    string r;

    in.seekg(0, std::ios::end);

    size_t sz = size_t(in.tellg());

    if ( sz > MAX_FILE_SIZE ) throw "File too big";

    r.reserve( sz );
    in.seekg(0, std::ios::beg);

    r.assign( std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() );

    return r;
}

bool ol::delfile(string fn)
{
    if ( !fs::exists(fn) ) return false;
    fs::remove(fn);
    return !fs::exists(fn);
}

int ol::bzip(string file, bool enc)
{
    auto fsz = fs::file_size(file);
    string cmd = "bzip2 ";
    if ( fsz > 5000000 ) cmd += "-v ";
    if ( !enc ) cmd += "-d ";
    cmd += file;
    std::cout << std::flush; std::cerr << std::flush;
    int rsys = std::system(cmd.c_str());
    return rsys;
}


ol::Msul ol::readdir()
{
    Msul r;
    auto cdir = fs::directory_iterator(".");
    for ( auto const & de : cdir )
    {
        string nm = de.path().filename().string();
        unsigned long long tc = de.last_write_time().time_since_epoch().count();
        long sz = -1L;
        if (!de.is_directory()) sz = (long)de.file_size();
        r[nm] = std::pair<unsigned long long, long>(tc, sz);
    }

    return r;
}

ol::Msul ol::Msul::dirs() const
{
    Msul r;
    for ( const auto & e : *this ) if (e.second.second < 0) r.insert(e);
    return r;
}

ol::Msul ol::Msul::files() const
{
    Msul r;
    for (const auto & e : *this) if (e.second.second >= 0) r.insert(e);
    return r;
}

ol::vs ol::Msul::names() const
{
    vs r;
    for ( const auto & e : *this ) r.push_back(e.first);
    return r;
}

unsigned long long ol::filetime(fs::path file)
{
    std::error_code err;
    auto lwt = 1ull * fs::last_write_time(file, err).time_since_epoch().count();
    bool ok = !err.default_error_condition();
    if ( !ok ) return 0;
    return lwt;
    // FIXME dir
}

bool ol::replaceAll(string & s, const string & r, const string & to)
{
    bool found = false;
    while (1)
    {
        size_t i = s.find(r);
        if ( i == string::npos ) return found;
        s.replace(i, r.size(), to);
        found = true;
    }
    throw 0; // never
}

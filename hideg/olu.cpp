#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

#include "olu.h"

namespace fs = std::filesystem;

bool ol::endsWith(string s, string fx)
{
    auto sz = s.size();
    auto fz = fx.size();
    return sz >= fz && 0 == s.compare(sz - fz, fz, fx);
};

string ol::file2str(const string & file)
{
    const size_t MAX_FILE_SIZE = 1024u * 1024 * 1000; // 1000Mb
    std::ifstream in(file.c_str(), std::ios::binary);

    if ( !in )
        return "";

    string r;

    in.seekg(0, std::ios::end);

    size_t sz = x2st(in.tellg());

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
    string cmd = "bzip2 -v ";
    if ( !enc ) cmd += "-d ";
    cmd += file;
    std::cout << std::flush; std::cerr << std::flush;
    int rsys = std::system(cmd.c_str());
    return rsys;
}


#pragma once

#include <string>
#include <filesystem>

namespace fs = std::filesystem;
using std::string;

namespace jadd
{
const auto HEADSZ = 500ull;
const unsigned long long ReadBlockSize = 10000;
// test block size
// 0 11.5    1 36.5    10 8.7   1000 5.2
// 10,000 5.0   100,000 5.2     1,000,000 5.4

extern string g_same_name;

struct File
{
    unsigned long long sz = 0;
    string hashHead;
    string hashFile;

    fs::path pth;
    unsigned long long tc = 0; // time

    void print() const;
    static bool same(fs::path dira, File & a, fs::path dirb, File & b);
    static void fillHash(fs::path dira, File & a, bool headonly);
};

struct DirNode
{
    fs::path dirName, fullName;
    ivec<int> idxs;  // files
    ivec< DirNode * > dirs; // intentional

    unsigned long long sz = 0;
    string hash;

    DirNode() {};
    DirNode(const DirNode &) = delete;
    void operator=(const DirNode &) = delete;
    ~DirNode() { while ( !dirs.empty() ) { delete dirs.back(); dirs.pop_back(); } }

    void print() const;
};

struct Files
{
    fs::path dir;
    ivec<File> files;
    std::map<unsigned long long, ivec<int> > sz2idx;

    DirNode * dirTree = nullptr;

    int add(File f)
    {
        int idx = files.size();
        sz2idx[f.sz].push_back(idx);
        files.push_back(f);
        return idx;
    }
    void print() const;
};

void loadFrom(fs::path dir, Files & f);
string fullHash(ol::ull sz, fs::path pfile);

} //jadd

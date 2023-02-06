#pragma once

#include <string>
#include <filesystem>

namespace fs = std::filesystem;
using std::string;

const bool ISOPEN = !false;

namespace g
{
extern fs::path gfexe; // arg0
extern int depth;
extern fs::path keyfile; // key file path
extern string sysuid;
extern string keyfilename; // filename only
extern string hkey; // key hex hash

extern fs::path dotgf; // ".gf"
extern bool recursive_mode;
extern fs::path cwd;
extern fs::path root_cwd;
inline fs::path rcwd() { return cwd.lexically_relative(root_cwd); }

extern int keepfile;
extern ivec<string> ignore;
extern bool silent;
} //g


namespace gfu
{
void find_depth();
void find_key();
string fileHash(string fname);
string dkey(int derivation_level);
void zpaq_unpack(string file, bool withkey);
bool ignored(string name);

int bzip(string file, bool pck, bool keep);
int zpaq(string file, bool pck, string key);
int cmix(string file, bool pck);
} // gfu


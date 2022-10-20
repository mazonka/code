#pragma once

#include <string>
#include <filesystem>

namespace fs = std::filesystem;
using std::string;

namespace g
{
extern fs::path gfexe; // arg0
extern int depth;
extern fs::path keyfile; // key file path
extern string sysuid;
extern string keyfilename; // filename only
extern string hkey; // key hex hash
} //g


namespace gfu
{
void find_depth();
void find_key();
string fileHash(string fname);
} // gfu
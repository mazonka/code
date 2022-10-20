#pragma once

#include <string>
#include <filesystem>

namespace fs = std::filesystem;
using std::string;

namespace g
{
extern fs::path gfexe; // arg0
} //g


// FIXME use namespace g
extern int g_depth;
extern fs::path g_keyfile; // key file path
extern string g_sysuid;
extern string g_keyfilename; // filename only
extern string g_hkey; // key hex hash

namespace gfu
{
void find_depth();
void find_key();
string fileHash(string fname);
} // gfu
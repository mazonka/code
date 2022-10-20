#pragma once

#include <string>
#include <filesystem>

namespace fs = std::filesystem;
using std::string;

// FIXME use namespace g
extern fs::path g_gfexe; // arg0
extern int g_depth;
extern fs::path g_keyfile; // key file path
extern string g_sysuid;
extern string g_keyfilename; // filename only
extern string g_hkey; // key hex hash

// FIXME namespace gfu
void find_depth();
void find_key();
string fileHash(string fname);

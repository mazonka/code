#pragma once

#include <string>
using std::string;

#include "olu.h"
///using vs = ivec<string>;

int main_bzc(ivec<string> args);
int main_test(ivec<string> args);
int main_hid(ivec<string> args);
int main_pack(ivec<string> args, bool pack, bool pkopen);
int main_fcl(ivec<string> args, bool keep);
int main_info(ivec<string> args);
int main_sync(ivec<string> args, int sync_co_st);
int main_zpaq(ivec<string> args);
int main_cmix(ivec<string> args);
int main_setpath(ivec<string> args);
int main_jadd(ivec<string> args);
int main_snap(ivec<string> args);
int main_same(ivec<string> args);
int main_vault(ivec<string> args);
int main_long(ivec<string> args);

ol::ull try_gfexe();

#pragma once

#include <string>
using std::string;

#include "olu.h"
///using vs = ivec<string>;

int main_bzc(ivec<string> args);
int main_test(ivec<string> args);
int main_hid(ivec<string> args);
int main_pack(ivec<string> args, bool pack);
int main_fcl(ivec<string> args, bool keep);
int main_info(ivec<string> args);
int main_sync(ivec<string> args, int sync_co_st);
int main_zpaq(ivec<string> args);
int main_cmix(ivec<string> args);
int main_setpath(ivec<string> args);
int main_fadd(ivec<string> args);

ol::ull try_gfexe();

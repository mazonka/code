#pragma once

#include <string>
#include <time.h>

using std::string;

namespace os
{
namespace FileSys
{
time_t mtime(const string & s);
}
} // os


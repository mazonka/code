#pragma once

#include "os.h"

#include <string>

using std::string;

namespace os
{

int kbhit();
void sleep(unsigned long msec);
void set_ip (string ip, string netmask, string broadcast, string gateway);
void console();
void restart_network_service();

unsigned codepage();
std::pair<int, int> termSize();
} // os


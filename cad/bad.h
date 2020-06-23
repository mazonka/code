#pragma once

#include <string>

#define BAD(x) if(1) if(bool(x)) throw ( std::string(__FILE__) + ":" + std::to_string(__LINE__) );
//#define BAD(x)

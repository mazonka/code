#include <thread>

using namespace std::chrono_literals;

#include "ccrun.h"

void cmain()
{
	while(1)
	{
		std::this_thread::sleep_for(100ms);
		break;
	}
}

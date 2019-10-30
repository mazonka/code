#include <iostream>
#include <windows.h>

int main()
{
	for ( int i=0; i<10; i++ )
	{
		Sleep(1000);
		 std::cout<<"\x7";
	}
}

#include <windows.h>
#include <conio.h>
#include <iostream>
#include <stdio.h>

#include "../gen/os.h"

int os::kbhit()
{
    int x = ::_kbhit();
    return x ?::_getch() : 0;
}

void os::console()
{
    system("cmd.exe");
}

void os::sleep(unsigned long msec)
{
    Sleep(msec);
}

unsigned os::codepage()
{
    return GetConsoleCP();
}

std::pair<int, int> os::termSize()
{
    static bool inited = false;
    static int width = 0;
    static int height = 0;

    if ( inited ) return std::make_pair(width, height);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    inited = true;

    return termSize();
}

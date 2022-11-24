#include <iostream>

#ifdef _MSC_VER
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#endif

int main()
{
#ifdef _MSC_VER
    std::cout << "Default page (ACP): " << GetACP() << '\n';
    std::cout << "Current page (CCP): " << GetConsoleCP() << '\n';
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int columns = w.ws_col;
    int rows = w.ws_row;
#endif
    std::cout << "Console : " << columns << "x" << rows << '\n';
}

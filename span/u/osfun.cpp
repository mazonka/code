#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
///#include <arpa/inet.h>
///#include <net/if.h>
///#include <net/route.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "../gen/os.h"

int os::kbhit()
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        //ungetc(ch, stdin);
        return ch;
    }

    return 0;
}

void os::console()
{
    system("bash");
}

void os::sleep(unsigned long msec)
{
    usleep(msec * 1000);
}

unsigned os::codepage()
{
    //return GetConsoleCP();
    return 1252;
}

std::pair<int, int> os::termSize()
{
    static bool inited = false;
    static int width = 0;
    static int height = 0;

    if ( inited ) return std::make_pair(width, height);

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    width = w.ws_col;
    height = w.ws_row;

    inited = true;

    return termSize();
}


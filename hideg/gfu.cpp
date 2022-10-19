#include "gfu.h"

int g_depth = -1;

void find_depth()
{
    if ( g_depth >= 0 ) return;
    auto fnd = []() -> int
    {
        string sp = ".";
        fs::path p(sp);
        for (int i = 0; i < 1000; i++)
        {
            fs::path cp(sp += "/..");
            if ( fs::equivalent(p, cp) ) return i;
            p = cp;
        }

        throw "Error in depth";
    };

    g_depth = fnd();
}

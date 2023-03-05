#include "ccrun.h"
#include "ccrut.h"


void cmain()
{
    for_( i, 16 )
    {
        for_ ( j, 16 )
        {
            auto k = i * 16 + j;
            if ( k <32 ) cout << '.';
            else cout << char((unsigned char)(unsigned)k);
        }
        cout << '\n';
    }
}
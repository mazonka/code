#include <iostream>
#include <cstdlib>
#include <string>
#include <chrono>
#include <thread>
#include <exception>

#include "chron.h"

using namespace std::chrono_literals;

using std::cout;

int main(int ac, const char * av[])
try
{
    if ( ac < 2 ) { std::cout << "Use: sizeMb or 0\n"; return 0; }

    int sz = std::atoi(av[1]);

    if ( sz > 0 )
    {

        for ( int i = 0; i < sz; i++ )
        {
            for ( int j = 0; j < 1024; j++ )
                void * p = new char[1024];

            cout << i + 1 << '\r';
        }

    }
    else
    {
		const size_t gb = 1024u*1024*1024;
        size_t x = 1;
        volatile char * p = nullptr;
        while (1)
        {
            std::cout << "Trying " << x << " Gb, "<<std::flush;
			auto start = chron::now();
            p = new(std::nothrow) char[x*gb];
            if (!p) break;
            for (auto i = x * 0; i < 10000; i++) p[i] = 'a';
            for (auto i = x * 0; i < 10000; i++) p[i+x*gb/2] = 'a';
            for (auto i = x * 0; i < 10000; i++) p[i+x*gb-10000] = 'a';
			auto end = chron::now();
            delete[]p;
			cout<<"time: "<<(end-start)<<" ms\n";
            ++x;
        }

        cout << "Not allocated " << x << " Gb at [" << ((void *)p) << "]\n";
        std::this_thread::sleep_for(1000ms);
        cout << "AAA1\n";
        for ( auto i = x * 0; i < x; i++ )
        {
            cout << "AAA b " << i << "\n";
            p[i] = 'a';
            cout << "AAA e " << i << "\n";
        }
        cout << "AAA2\n";
        cout << "2Allocated " << x << " Gb at " << ((void *)p) << '\n';
        cout << "AAA3\n";
        std::this_thread::sleep_for(5000ms);

    }
}
catch (...)
{
    cout << "error\n";
    return 1;
}

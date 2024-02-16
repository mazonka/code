#include "ccrun.h"
#include "../../hideg/olu.h"
#include "../../1file/cpp2/coutlog.h"

#include <thread>
#include <mutex>
#include <algorithm>
std::mutex g_mutex;

using num = unsigned long long;

ivec<int> digits(num x)
{
    ivec<int> r;
    while (x)
    {
        r += int(x % 10);
        x /= 10;
    }
    return r;
}

inline num pw(num x, int j)
{
    if (j == 0) return 1;
    if (j == 1) return x;
    num z = 1;
    for (int i = 0; i < j; i++) z *= x;
    return z;
}

int init(num v[])
{
    for ( int j = 0; j <= 15; j++ )
        for ( int x = 0; x < 10; x++ )
            v[j * 10 + x] = pw(x, j);

    return v[10];
}

inline num pwt(int x, int j)
{
    static num v[10 * 20];
    static int dummy = init(v);
    return v[j * 10 + x];
}

inline bool pattern(const ivec<int> & ds, const ivec<int> & p)
{
    auto i = std::search(ds.begin(), ds.end(), p.begin(), p.end());
    if ( i == ds.end() ) return false;
    return true;
}

void func(int in, int sz)
{
    //std::vector<int> vpat{ 1,0,4 };
    //ivec<int> pat{ vpat };

    for ( num i = 11 + in; i < 1'000'000'000'000ull; i += sz )
    {
        auto ds = digits(i);

        if ( !(i % 10'000'000) )
        {
            std::lock_guard<std::mutex> guard(g_mutex);
            std::cerr << ":" << (i/1'000'000) << "\r";
        }

	if(0)
	{
	if( ds.size() < 9 ) continue;
	if( ds[0] != 8 ) continue;
	if( ds[1] != 3 ) continue;
	if( ds[2] != 1 ) continue;
	if( ds[2] != 0 ) continue;
	}

        //if ( !pattern(ds, pat) ) continue;

        for (int j = 2; j <= 15; j++)
        //for (int j = 13; j <= 13; j++)
        {
            num sum = 0; for ( auto x : ds ) sum += pwt(x, j);
            if ( i == sum )
            {
                std::lock_guard<std::mutex> guard(g_mutex);
                cout << "n=" << i << " pw=" << j << " sum=" << sum << '\n';
            }
        }

    }
}

void cmain()
{
    int batch_size = 8;
    ivec<std::thread> threads;

    for_i(batch_size) threads.emplace_back(std::thread {&func, i, batch_size});

    for ( auto & t : threads ) t.join();
}

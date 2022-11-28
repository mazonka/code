#include "crun.h"
#include "../../hideg/olu.h"
#include "../../1file/cpp2/coutlog.h"

#include <thread>
#include <mutex>
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

void func(int in, int sz)
{
    for ( num i = 11 + in; i < 1'000'000'000'000ull; i += sz )
    {
        auto ds = digits(i);
        //num sum1 = ds.add(0);
        //num sum2(0); for( auto x:ds ) sum2 += x*x;
        //num sum3(0); for( auto x:ds ) sum3 += x*x*x;
        //cout<<i<<ds.str(" ")<<" sum1="<<sum1<<" sum2="<<sum2<<'\n';
        //if( i==sum2 ) cout<<"AAA sum2 "<<i<<'\n';
        //if( i==sum3 ) cout<<"AAA sum3 "<<i<<'\n';
        //cout<<i<<' '<<sum2<<' '<<sum3<<'\n';

        for (int j = 2; j <= 15; j++)
        {
            num sum = 0; for ( auto x : ds ) sum += pwt(x, j);
            if ( i == sum )
            {
                std::lock_guard<std::mutex> guard(g_mutex);
                cout << "n=" << i << " pw=" << j << " sum=" << sum << '\n';
            }
        }

        if ( !(i % 10'000'000) )
        {
            std::lock_guard<std::mutex> guard(g_mutex);
            std::cerr << ":" << i << "\r";
        }
    }
}

void cmain()
{
    int batch_size = 10;
    ivec<std::thread> threads;

    for_i(batch_size) threads.emplace_back(std::thread {&func, i, batch_size});

    for ( auto & t : threads ) t.join();
}

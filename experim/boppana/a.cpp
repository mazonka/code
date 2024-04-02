#include <cmath>
#include <iomanip>

#include "ccrun.h"
#include "coutlog.h"

/*
  |----------------+----------|
    | Fraction       | Decimals |
    |----------------+----------|
    | 3              |      0.8 |
    | 22/7           |      2.9 |
    | 333/106        |      4.1 |
    | 355/113        |      6.6 |
    | 103993/33102   |      9.2 |
    | 104348/33215   |      9.5 |
    | 208341/66317   |      9.9 |
    | 312689/99532   |     10.5 |
    | 833719/265381  |     11.1 |
    | 1146408/364913 |     11.8 |
    |----------------+----------|
more on https://en.wikipedia.org/wiki/Approximations_of_%CF%80
see also
https://mathworld.wolfram.com/HarmonicSeries.html
https://www.youtube.com/watch?v=HtX6T0njHzQ

2.163 for 10^7
*/

double pi = std::acos(-1);

void cmain()
{
    // compute orig
    // [orig for N=100000000 is 2.16306]
    int N = 1300000;
    double sum = 0;
    for ( int i = 1; i <= N; i++ )
    {
        double s = (std::sin(1.0 * i) + 2) / 3;
        double ps = std::pow(s, i);
        double term = ps / i;
        sum += term;
    }

    //cout << "orig for N=" << N << " is " << sum << '\n';

    void step01(); step01();
    void step02(); step02();
    void step03(); step03();
}

void step01()
{
    // step 1
    // use p as double - this must give exact as the original

    int N = 100;
    int q = 103993; // q/p = 2pi

    double dp = q / (2 * pi);
    double p = std::round(dp);

    double sum = 0;
    for ( int k = 0; k < N; k++ )
    {
        for ( int i = 1; i <= q; i++ )
        {
            int n = q * k + i;
            double si = (std::sin(2 * pi * i * p / q) + 2) / 3;
            double ps = std::pow(si, n);
            double term = ps / n;
            sum += term;
        }
    }

    //cout << __func__ << ": " << sum << '\n';
}

void step02()
{
    // step 2
    //

    long long unsigned N = 1000'000'000;
    // q/p = 2pi
    //long long unsigned q = 103993;
    //long long unsigned q = 411557987;
    long long unsigned q = 1001;

    double dp = q / (2 * pi);
    double p = std::round(dp);

    double sum = 0;
    for (long long unsigned i = 1; i <= q; i++ )
    {
        double arg1 = 1.0 * i * p / q;
        double arg2 = 2 * pi * arg1;
        double si = (std::sin(arg2) + 2) / 3;
        for (long long unsigned k = 0; k < N; k++ )
        {
            long long unsigned n = q * k + i;
            double ps = std::pow(si, n);
            double term = ps / n;
            double sum0 = sum;
            sum += term;
            if (term < 0)
                never;
            if ( sum == sum0 )
                break;
        }
    }

    //cout << __func__ << ": " << std::setprecision(20) << sum << '\n';
}


void step03()
{
    // step 2
    //

    long long unsigned N = 10000'000'000;
    // q/p = 2pi
    //long long unsigned q = 103993;
    //long long unsigned q = 411557987;
    long long unsigned q = 1001;

    //for ( long long unsigned q = 200; q < 100'000'000; ++q )
    for ( long long unsigned q = 130000; q < 100'000'000; ++q )
    {
        double dp = q / (2 * pi);
        double p = std::round(dp);
        double delta = std::abs(p - dp);
        //if (  delta > 0.01 ) continue;
        if (  delta > 0.0001 ) continue;

        //cout << q;

        double sum = 0;
        for (long long unsigned i = 1; i <= q; i++ )
        {
            double arg1 = 1.0 * i * p / q;
            double arg2 = 2 * pi * arg1;
            double si = (std::sin(arg2) + 2) / 3;
            for (long long unsigned k = 0; k < N; k++ )
            {
                long long unsigned n = q * k + i;
                double ps = std::pow(si, n);
                double term = ps / n;
                double sum0 = sum;
                sum += term;
                if (term < 0)
                    never;
                if ( sum == sum0 )
                    break;
            }
        }

        sum -= 2.163;
        ostringstream os;
        os << std::setprecision(20) << sum;
        cout << q << '\t' << os.str() << '\t' << delta * 100 << '\n';
    }

    //cout << __func__ << ": " << std::setprecision(20) << sum << '\n';
}

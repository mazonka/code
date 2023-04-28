#include <cstdlib>
#include <cmath>

#include <iostream>
#include <fstream>
#include <vector>

#include <string>
#include <exception>
#include <cmath>

#include "dlibad.h"

using std::cout;
using std::string;

typedef std::vector<double> vd;

struct Data
{
    int L = 3;
    int K = 5;
    int N = 5;
    double frobfact = 0.00001;

    int size() const { return N * K + K * L; }
    //int size() const { return N * K + K * L * 2; }

    double gamma(const double * x, int n, int k) const
    {
        return x[k + n * K];
    }

    double aki(const double * x, int k, int i) const
    {
        return x[N * K + i + k * L];
    }

    double bkj(const double * x, int k, int j) const
    {
        return aki(x, k, j);
        return x[N * K + L * K + j + k * L];
    }

    double gab(const double * x, int n, int k, int i, int j) const
    {
        return gamma(x, n, k) * aki(x, k, i) * bkj(x, k, j);
    }
};

const Data g_data;


double func(int nx, const double * x, const void * cv_data)
{
    const Data & data = *static_cast<const Data *>(cv_data);
    const int L = data.L;
    const int K = data.K;
    const int N = data.N;


    if ( nx !=  data.size() ) throw "Bad vector size";

    auto sq = [](double y) -> double { return y * y; };

    auto eqLHS = [](int n, int i, int j) -> double
    {
        return (i + j == n) ? 1 : 0;
    };

    auto eqRHS = [&](int n, int i, int j) -> double
    {
        double sum = 0;
        for ( int k = 0; k < K; k++ )
        {
            //sum += gamma(x, n, k) * aki(k, i) * bkj(k, j);
            sum += data.gab(x, n, k, i, j);
        }//k
        return sum;
    };

    double r = 0;

    for ( int n = 0; n < N; n++ )
    {
        for ( int i = 0; i < L; i++ )
        {
            for ( int j = 0; j < L; j++ )
            {
                r += sq( eqLHS(n, i, j) - eqRHS(n, i, j) );
            }//j
        }//i
    }//n

    double sum2 = 0;
    for ( int i = N * K; i < nx; i++ ) sum2 += sq(x[i] - std::round(x[i]));
    r += data.frobfact * sum2 / (2 * L * K);

    if (1)
    {
        static int ctr = 0;
        if ( ++ctr > 10000 )
        {
            ctr = 0;
            std::cout << r << '\n';
        }
    }

    //std::cout <<"AAA r=" << r << '\n';
    return r;
}


vd xmain(const vd & v, Dlibad::typef fn)
{
    Dlibad d(fn, (const void *)&g_data);
    vd r = d.solve(v, 1000000);
    return r;
}

int main(int argc, char ** argv)
try
{
    vd a;
    while ( a.size() < g_data.size() ) a.push_back(0.001);

    a = xmain(a, func);
    for ( size_t i = 0; i < a.size(); i++ ) std::cout << ' ' << a[i];
    std::cout << "\nF=" << func(a.size(), a.data(), &g_data) << '\n';

    const int L = g_data.L;
    const int K = g_data.K;
    const int N = g_data.N;

    cout << "gamma=\n";
    for ( int n = 0; n < N; n++ )
    {
        for ( int k = 0; k < K; k++ )
        {
            cout << '\t' << g_data.gamma(a.data(), n, k);
        }//k
        cout << '\n';
    }//n

    cout << "alpha=\n";
    for ( int k = 0; k < K; k++ )
    {
        for ( int i = 0; i < L; i++ )
        {
            cout << '\t' << g_data.aki(a.data(), k, i);
        }//k
        cout << '\n';
    }//n

    if (0)
    {
        cout << "b=\n";
        for ( int k = 0; k < K; k++ )
        {
            for ( int j = 0; j < L; j++ )
            {
                cout << '\t' << g_data.bkj(a.data(), k, j);
            }//k
            cout << '\n';
        }//n
    }

    //std::ofstream of("test.dat");
    //of << "0 0\n";
    //for ( size_t i = 0; i < a.size(); i += 2 )
    //    of << a[i] << ' ' << a[i + 1] << '\n';
    //of << "1 0\n";
}
catch (int e)
{
    cout << "error (int): " << e << "\n";
    return 1;
}
catch (string e)
{
    cout << "error: " << e << "\n";
    return 1;
}
catch (const char * e)
{
    cout << "error: " << e << "\n";
    return 1;
}
catch (std::exception & e)
{
    cout << "error (std): " << e.what() << "\n";
    return 1;
}
catch (...)
{
    cout << "Unknown exception\n";
    return 1;
}


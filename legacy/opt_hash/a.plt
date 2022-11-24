
set logscale y

#             n^N - (n-1)^N
#    k*p1 = M _____________
#                n^(N+1)

N = 100

f(x) = ( x**N - (x-1)**N ) / x**(N+1)
g(x) = ( 1 - (1-1/x)**N ) / x

plot [1:200] g(x)

pause 100


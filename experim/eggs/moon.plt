#f(x)=6.67e-11*5.97e24*86400**2 / (2*pi*6.371e6)**2/1000000

plot [100:] "a.dat" u 1:2 w l
replot "a.dat" u 1:3 w l
replot "a.dat" u 1:4 w l
#replot f(x) w l

pause 1000

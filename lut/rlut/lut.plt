set size square
plot [-1.1:1.1] [-1.1:1.1] "lut.edg.dat" w l
replot "lut.pnt.dat" pt 7 lc 3 ps 0.1

pause 1000

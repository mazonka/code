set size square
load "o/labels"
plot [-2:2] [-2:2] "o/wall.dat" u 1:2 w l t ""
#replot "lut.pnt.dat" pt 7 lc 3 ps 0.1

pause 1000

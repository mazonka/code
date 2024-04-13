plot "cout.log" w l

sc=50

#replot "cout.log" u 1:($3/sc) w p

#replot "cout.log" u 1:($3/sc) w l
replot "cout.log" w p

pause 1000

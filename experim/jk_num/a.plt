set size ratio -1

set offset graph 0.1, graph 0.1, graph 0.1, graph 0.1

plot "a1.dat" w p pt 5 lt 3 t ""
replot "a2.dat" w p pt 5 lt 2 t ""
replot "a3.dat" w p pt 5 lt 1 t ""

pause 1000
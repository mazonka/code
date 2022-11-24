rm -f lut.dat lut.edg.dat lut.pnt.dat
::lut build
::lut draw lut 0
lut debug lut -1
call plot lut.plt


all: lut.exe

lut.exe: lut.obj main.obj alg.obj f.obj cube.obj
	cl -EHsc $^ -Felut

main.obj: main.cpp *.h
	cl -EHsc -c $<

alg.obj: alg.cpp *.h
	cl -EHsc -c $<

lut.obj: lut.cpp *.h
	cl -EHsc -c $<

f.obj: f.cpp f.h
	cl -EHsc -c $<

cube.obj: cube.cpp *.h
	cl -EHsc -c $<

clean:
	rm -f *.obj *.exe

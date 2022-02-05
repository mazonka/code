all : crun.exe

crun.exe: crun.cpp
	cl -nologo -EHsc -Ox -std:c++17 crun.cpp -Fecrun.exe
	rm -f crun.obj
#	g++ -O3 -std=c++17 crun.cpp -o crun.exe

c clean:
	rm -rf cache crun.exe

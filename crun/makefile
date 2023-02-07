all : ccrun.exe

ccrun.exe: ccrun.cpp
	cl -nologo -EHsc -Ox -std:c++17 ccrun.cpp -Feccrun.exe
	rm -f ccrun.obj
#	g++ -O3 -std=c++17 ccrun.cpp -o ccrun.exe

c clean:
	rm -rf cache ccrun.exe

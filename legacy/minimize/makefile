all: a.exe b.exe

a.exe: alglib/a.lib a.cpp
	cl -EHsc a.cpp alglib/a.lib

b.exe: alglib/a.lib b.cpp
	cl -EHsc b.cpp alglib/a.lib

alglib/a.lib:
	cd alglib && make -f ../lib.mak

clean:
	rm -f alglib/*.obj alglib/a.lib a.exe a.obj
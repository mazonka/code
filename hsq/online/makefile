
all: hsqjs.cgi
	chmod 0777 hsqjs.cgi

hsqjs.cgi: hsqjs.cpp cgi.h cgi.obj
	cl -EHsc hsqjs.cpp cgi.obj -Fehsqjs.cgi

cgi.obj: cgi.cpp cgi.h
	cl -EHsc -c cgi.cpp

clean:
	rm -rf *.obj *.log hsqjs.cgi
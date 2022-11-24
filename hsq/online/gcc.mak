
all: hsqjs.cgi
	chmod 0755 hsqjs.cgi

hsqjs.cgi: hsqjs.cpp cgi.h cgi.cpp
	g++ -O2 hsqjs.cpp cgi.cpp -o hsqjs.cgi -static-libstdc++


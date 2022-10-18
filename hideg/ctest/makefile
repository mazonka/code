all: key *.cpp

key:
	../gf.exe bzc genkey .gf.key < gkey.txt

touch:
	touch *.cpp

test2.cpp: ../hash.h ../hash.cpp ../sha2.cpp
	touch $@
	

c clean:
	rm -f .gf.key *.tmp


run:
	run.sh

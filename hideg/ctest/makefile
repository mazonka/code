all: *.cpp key

key:
	../gf.exe bzc genkey .gf.key < gkey.txt

test1.cpp:
	touch $@

test2.cpp: ../hash.h ../hash.cpp ../sha2.cpp
	touch $@
	

c clean:
	rm -f .gf.key *.tmp


run:
	run.sh

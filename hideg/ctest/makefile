all: *.cpp bzc.key

bzc.key:
	../bzc.exe genkey bzc.key < gkey.txt

test1.cpp:
	touch $@

test2.cpp: ../hash.h ../hash.cpp ../sha2.cpp
	touch $@
	

c clean:
	rm -f bzc.key *.tmp


run:
	run.sh

all: key *.cpp

key k:
	../gf.exe bzc genkey .gf.key < gkey.txt

touch:
	touch *.cpp

test2.cpp: ../hash.h ../hash.cpp ../sha2.cpp
	touch $@
	
test4.cpp t05.cpp: ../hash.h ../hash.cpp ../sha2.cpp ../olu.*
	touch $@


c clean:
	rm -f .gf.key *.tmp


run:
	run.sh

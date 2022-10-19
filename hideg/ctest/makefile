all: key t*.cpp

key k:
	../gf.exe bzc genkey .gf.key < gkey.txt

touch:
	touch *.cpp


t*.cpp: ../hash.h ../hash.cpp ../sha2.cpp ../olu.*
	touch $@


c clean:
	rm -f .gf.key *.tmp


run:
	run.sh

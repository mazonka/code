all: key t*.cpp

key k:
	../gf.exe bzc genkey .gf.key < gkey.txt

touch:
	touch *.cpp


t*.cpp: ../hash.h ../hash.cpp ../sha256.cpp ../olu.*
	touch $@


c clean:
	rm -rf .gf.key *.tmp .gf


run:
	run.sh

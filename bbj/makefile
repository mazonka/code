#COMPIL=bc.bat
#COMPIL=dm.bat -Ae
COMPIL=cl -EHcs -Ox
#COMPIL=g++ -O2 -o $@

all: bbjasm.exe bbjrun.exe

bbjasm.exe: bbjasm.cpp
	$(COMPIL) bbjasm.cpp
	rm -f bbjasm.obj

bbjrun.exe: bbjrun.cpp
	$(COMPIL) bbjrun.cpp
	rm -f bbjrun.obj

clean:
	rm -rf *.obj *.exe *.run *.tds *.map *.tmp

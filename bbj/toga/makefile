#COMPIL=bc.bat
#COMPIL=dm.bat -Ae
COMPIL=cl -EHcs -Ox
#COMPIL=g++ -O2 -o $@

all: teasm.exe terun.exe

teasm.exe: teasm.cpp
	$(COMPIL) teasm.cpp
	rm -f teasm.obj

terun.exe: terun.cpp
	$(COMPIL) terun.cpp
	rm -f terun.obj

clean:
	rm -rf *.obj *.exe *.tds *.map *.tmp

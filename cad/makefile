O=o

all: st $O house.exe

$O:
	mkdir -p $O

house.exe: $O/house.obj $O/model.obj $O/main.obj
	cl -nologo $(INCD) $^ $(LIBS) -Fe$@

$O/house.obj: house.cpp *.h
	cl -nologo /EHsc -Ox -c $(INCD) $< -Fo$@

$O/model.obj: model.cpp *.h
	cl -nologo /EHsc -Ox -c $(INCD) $< -Fo$@

$O/main.obj: main.cpp *.h
	cl -nologo /EHsc -Ox -c $(INCD) $< -Fo$@

clean c:
	@rm -f *.exe *.obj *.log
	@rm -rf o

st:
	@style.bat *.cpp *.h

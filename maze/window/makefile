
WX=C:/wxWidgets-2.8.11

INC=-I $(WX)/include/msvc -I $(WX)/include
LIBPATH=$(WX)/lib/vc_lib

LIBS="comctl32.lib" "Rpcrt4.lib" "kernel32.lib" "user32.lib" "gdi32.lib" \
	"winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" \
	"oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib"

# USE STATIC OR DYNAMIC LIBS
STATICLIB=-MT
#DYNAMICLIB=-MD
#NOSTATICLIB=/NODEFAULTLIB:LIBCMT

# USE UNICODE
#UNICODE=-D_UNICODE=1

OPT=-EHsc -Ox

all: mazew.exe

mazew.exe: mazew.obj maze.obj draw.obj solver.obj generate.obj \
	../common/*.h ../common/*.cpp
	link $(NOSTATICLIB) mazew.obj maze.obj draw.obj solver.obj \
	generate.obj /LIBPATH:$(LIBPATH) $(LIBS)

mazew.obj: mazew.cpp mazew.h ../common/*.h
	cl -c $(OPT) -I ../common $(INC) $(STATICLIB) $(DYNAMICLIB) $(UNICODE) $<

maze.obj: ../common/maze.cpp ../common/maze.h
	cl -c $(OPT) -I ../common $(INC) $(STATICLIB) $(DYNAMICLIB) $(UNICODE) $<

draw.obj: draw.cpp
	cl -c $(OPT) -I ../common $(INC) $(STATICLIB) $(DYNAMICLIB) $(UNICODE) $<

solver.obj: ../common/solver.cpp ../common/*.h
	cl -c $(OPT) -I ../common $(INC) $(STATICLIB) $(DYNAMICLIB) $(UNICODE) $<

generate.obj: ../common/generate.cpp ../common/*.h
	cl -c $(OPT) -I ../common $(INC) $(STATICLIB) $(DYNAMICLIB) $(UNICODE) $<

clean:
	rm -f mazew.exe.manifest mazew.exe *.obj *.tmp

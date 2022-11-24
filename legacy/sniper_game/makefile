
CL=cl
OP=-EHsc -Ox -WX

all: game.exe game2.exe

game.exe: game.cpp; $(CL) $(OP) $<
game2.exe: game2.cpp; $(CL) $(OP) $<

clean:; rm -f *.obj *.exe *.out *.tmp

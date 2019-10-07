SRC=./
include ./mk_all.mak

O=$(TRG)


src = sam.cpp os_filesys.cpp gl_except.cpp link.cpp \
gl_utils.cpp ma_hash.cpp sha2.cpp index.cpp \
olmain.cpp copyfile.cpp olc.cpp samf.cpp

obj := $(src:%.cpp=o/%.obj)

srcp = os_file2.cpp timerp.cpp os_mtime.cpp osfun.cpp

objp := $(srcp:%.cpp=o/%.obj)

#all: o same.exe sameh.exe sam.exe
all: o sam.exe

o:
	mkdir -p o

sam.exe: $(obj) $(objp)
	$(COMPILER) $(obj) $(objp) $(EOUT)$@

$(obj): o/%.obj:%.cpp *.h
	$(COMPILER) -c $(OPT) $< $(OOUT)$@

$(objp): o/%.obj:$(PLAT)/%.cpp *.h
	$(COMPILER) -c $(OPT) -I. $< $(OOUT)$@


c clean:
	rm -rf *.exe o

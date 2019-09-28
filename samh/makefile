
src = os_filesys.cpp gl_except.cpp samehf.cpp \
gl_utils.cpp ma_hash.cpp sha2.cpp \
olmain.cpp sameh.cpp copyfile.cpp

obj := $(src:%.cpp=o/%.obj)

srcp = os_file2.cpp timerp.cpp os_mtime.cpp

objp := $(srcp:%.cpp=o/%.obj)

#all: o same.exe sameh.exe sam.exe
all: o samh.exe

o:
	mkdir -p o

#same.exe: o/same.obj $(obj)
#	cl $< $(obj)

#sameh.exe: o/sameh.obj $(obj)
#	cl $< $(obj)

samh.exe: o/samh.obj $(obj) o/olmain.obj $(objp)
	cl $< $(obj) $(objp)

o/samh.obj: samh.cpp *.h
	cl -c -Ox -EHsc -I../ol $< -Fo$@

#o/olmain.obj: olmain.cpp *.h
#	cl -c -Ox -EHsc -I../ol $< -Fo$@

#o/same.obj: same.cpp *.h
#	cl -c -Ox -EHsc -I../ol $< -Fo$@

#o/sameh.obj: sameh.cpp *.h
#	cl -c -Ox -EHsc -I../ol $< -Fo$@

$(obj): o/%.obj:%.cpp *.h
	cl -c -Ox -EHsc -I../ol -D_USING_V110_SDK71_=1 $< -Fo$@

$(objp): o/%.obj:w/%.cpp *.h
	cl -c -Ox -EHsc -I../ol -I. -D_USING_V110_SDK71_=1 $< -Fo$@


c clean:
	rm -rf *.exe o


src = os_file2.cpp os_filesys.cpp gl_except.cpp samehf.cpp \
gl_utils.cpp timer_w.cpp ma_hash.cpp sha2.cpp os_mtime.cpp \
olmain.cpp sameh.cpp copyfile.cpp

obj := $(src:%.cpp=o/%.obj)

#all: o same.exe sameh.exe sam.exe
all: o sam.exe

o:
	mkdir -p o

#same.exe: o/same.obj $(obj)
#	cl $< $(obj)

#sameh.exe: o/sameh.obj $(obj)
#	cl $< $(obj)

sam.exe: o/sam.obj $(obj) o/olmain.obj
	cl $< $(obj)

o/sam.obj: sam.cpp *.h
	cl -c -Ox -EHsc -I../ol $< -Fo$@

#o/olmain.obj: olmain.cpp *.h
#	cl -c -Ox -EHsc -I../ol $< -Fo$@

#o/same.obj: same.cpp *.h
#	cl -c -Ox -EHsc -I../ol $< -Fo$@

#o/sameh.obj: sameh.cpp *.h
#	cl -c -Ox -EHsc -I../ol $< -Fo$@

$(obj): o/%.obj:%.cpp *.h
	cl -c -Ox -EHsc -I../ol -D_USING_V110_SDK71_=1 $< -Fo$@


clean:
	rm -rf *.exe o

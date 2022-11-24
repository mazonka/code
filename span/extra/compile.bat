del span.exe
set OPT=-WX -W4 -nologo -EHsc -Ox -D_CRT_SECURE_NO_DEPRECATE=1 /wd4355 /wd4127 /wd4702 /wd4100 /wd4290 -D_USING_V110_SDK71_=1
cl %OPT% span.cpp msc/osfun.cpp  gen/gl_except.cpp gen/util.cpp msc/os_timer2.cpp msc/os_file2.cpp gen/os_filesys.cpp gen/gl_utils.cpp gen/quest.cpp
del *.obj
::pause
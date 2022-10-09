#!/bin/sh

rm -f span.exe
OPT=-std=c++14
g++ $OPT span.cpp unx/osfun.cpp  gen/gl_except.cpp gen/util.cpp unx/os_timer2.cpp unx/os_file2.cpp gen/os_filesys.cpp gen/gl_utils.cpp gen/quest.cpp -o span.exe

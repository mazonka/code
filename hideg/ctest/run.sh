#!/bin/bash

test -f ../gf.exe || { echo no gf.exe, build; exit 1; }

make

for i in t*.cpp
do
echo $i
#crun $i
crun $i 2>&1 | grep "OK\|FAIL\|error"
done


make c

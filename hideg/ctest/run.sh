#!/bin/bash

make

for i in *.cpp
do
#echo $i
#crun $i
crun $i 2>&1 | grep "OK\|FAIL"
done


make c

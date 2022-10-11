#!/bin/bash

make

for i in *.cpp
do
#echo $i
#crun $i
crun $i | grep "OK\|FAIL"
done


make c

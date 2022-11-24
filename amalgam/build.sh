#!/bin/sh
fcl3 make -D abc abc.fcl

echo "/*" > abc.cpp
cat abc.fcl >> abc.cpp
echo "*/" >> abc.cpp

rm -f abc.fcl
rm -f abc.exe

g++ -O2 -std=c++14 abc.cpp -o abc.exe && rm -f abc.cpp

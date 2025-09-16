#!/bin/bash

echo HOME=$HOME

mkdir -p $HOME/bin
cp ccrun.exe $HOME/bin/ccrun
echo "ccrun.exe copied to $HOME/bin"

mkdir -p $HOME/ccrun
cat compile.ccrun | grep g++ | sed 's/#//' > $HOME/ccrun/compile.ccrun
echo "compile.ccrun created in $HOME/ccrun"

cp ccrun.h $HOME/ccrun/
echo "ccrun.h copied to $HOME/ccrun"

echo "Please edit [$HOME/bin/ccrun/compile.ccrun] to set up the default compilation"



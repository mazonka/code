#!/bin/bash

echo HOME=$HOME

mkdir -p $HOME/bin
cp ccrun.exe $HOME/bin/ccrun
echo "ccrun.exe copied to $HOME/bin"

mkdir -p $HOME/ccrun
cp compile.ccrun $HOME/ccrun/
echo "compile.ccrun copied to $HOME/ccrun"

echo "Please edit [$HOME/bin/ccrun/compile.ccrun] to set up the default compilation"



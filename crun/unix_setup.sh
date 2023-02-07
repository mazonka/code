#!/bin/bash

echo HOME=$HOME

mkdir -p $HOME/bin
cp ccrun.exe $HOME/bin/ccrun
mkdir -p $HOME/ccrun
cp compile.ccrun $HOME/bin/ccrun/

echo "Please edit [$HOME/bin/ccrun/compile.ccrun] to set up the default compilation"



#!/bin/sh

nm=dlib.tar.bz2
artdir=../../../artill/src/

artlib=$artdir$nm

if test -f $artlib ; then
cp $artlib ./

else

z=https://github.com/mazonka/artill/raw/master/src/$nm
wget --no-check-certificate $z

fi

 
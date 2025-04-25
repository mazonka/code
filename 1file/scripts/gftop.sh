#!/bin/bash


cdir=`pwd`
cdir="${cdir%/}"             # strip trailing slash (if any)
subdir="${cdir##*/}"
echo $subdir
#exit
dir=$subdir

echo gftop on $dir

ups=`gf v up`
top=`gf v top`
dat=`date +%y%m%d%H%M%S`

echo $ups

tai=`tail -1 ../$dir.gftop`
new="$dat $top"

taiarr=($tai)
taih=${taiarr[1]}

newarr=($new)
newh=${newarr[1]}

#echo [$taih] [$newh]

if [ "$taih" = "$newh" ]; then
:
else
 echo $dat $top
 echo $dat $top >> ../$dir.gftop
fi


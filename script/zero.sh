#!/bin/bash

#  this script prints files that start with 0x00 or empty or non-readable

##IFS='\n'

fun(){
for i in *
do
if test -d "$i"; then
  pushd "$i" > /dev/null
  fun "$1/$i"
  popd > /dev/null
else
  zero=`hexdump -e '1/1 "%02.2x"' -n 1 "$i"`

  if [ "$zero" = "00" ] ; then
    echo "[00] $1/$i"
  fi

  if [ "$zero" = "" ] ; then
    echo "[  ] $1/$i"
  fi

  #echo "$1/$i" "[$zero]"
fi
done
}

fun .
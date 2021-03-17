#!/bin/bash

#  this script prints files that start with 0's or empty or non-readable

##IFS='\n'

shopt -s nullglob

fun(){
for i in *
do
if test -d "$i"; then
  pushd "$i" > /dev/null
  fun "$1/$i"
  popd > /dev/null
  echo -n -e "[$1]  \\r"
else
  zero=`hexdump -e '4/1 "%02.2x"' -n 4 "$i"`

  if [ "$zero" = "00000000" ] ; then
    echo "[0] $1/$i"
  fi

  if [ "$zero" = "" ] ; then
    echo "[ ] $1/$i"
  fi

  #echo -n -e "$1/$i [$zero]      \\r"
fi
done
}

fun .
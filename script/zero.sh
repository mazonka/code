#!/bin/bash

#  this script prints files that start with 0's or empty or non-readable

##IFS='\n'

zlog=`pwd`/zero.log

shopt -s nullglob

prn(){
echo -n -e "${old//?/.}\\r"
echo -n -e "$1\\r"
old=$1
}

log(){
    echo "$1"
    echo "$1" >> $zlog
}

fun(){
for i in *
do
if test -d "$i"; then
  prn "[$1/$i]"
  pushd "$i" > /dev/null
  fun "$1/$i"
  popd > /dev/null
else
  zero=`hexdump -e '4/1 "%02.2x"' -n 4 "$i"`

  if [ "$zero" = "00000000" ] ; then
    log "Z $1/$i"
  fi

  if [ "$zero" = "" ] ; then
    log "E $1/$i"
  fi

  #echo -n -e "$1/$i [$zero]      \\r"
fi
done
}

fun .
#!/bin/bash

# recursively goes through directories and renames files replacing ' '->_

IFS="\n"

cwd=.

file_report()
{
file=$1
#if [ -f "$file" ] ; then
  if [ "$file" = "${file//[[:space:]]/_}" ] ; then
    :
  else 
    echo $cwd/"$file"
  fi
#fi
}

dir_final()
{
for file in *;
do
if [ -f "$file" ] ; then
 file_report "$file"
fi
done
}

dir_rec()
{
local cw=$cwd
for dir in *;
do
  if [ -d "$dir" ] ; then
    file_report "$dir"
    cwd=$cw/"$dir"
    pushd "$dir" 2>&1 > /dev/nul
    dir_final
    dir_rec "$dir"
    popd 2>&1 > /dev/nul
    cwd="$cw"
  fi
done
cwd=$cw
}

#dir_final
dir_rec .

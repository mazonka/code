#!/bin/bash

# recursively goes through directories and renames files replacing ' '->_

IFS="\n"


dir_final()
{
for file in *;
do
test -f "$file" && mv -i "$file" "${file//[[:space:]]/_}"
done
}

dir_rec()
{
for dir in *;
do
  if [ -d "$dir" ] ; then
    pushd "$dir"
    dir_final
    dir_rec "$dir"
    popd
  fi
done
}

#dir_final
dir_rec .

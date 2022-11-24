#!/bin/bash

c=
test -f "$1" && c=FILE

if [ -d "$1" ] ; then
c=DIR

if cd "$1" ; then
test -z "$(ls -A)" && c=EMPTY
cd ..
fi

echo "item: $2/$1 - $c" 
fi

if [ "$c" = "EMPTY" ] ; then 
echo "item: $2/$1 - $c - REMOVING" 
rmdir "$1"
fi




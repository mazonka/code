#!/bin/bash

# This script visits all directories and
# for each file/dir calls 1st arg
# $ sh visitor.sh my.sh
# called: my.sh arg1 arg2
# arg1 - filename or dirname
# arg2 - relative current path

if [ "$1" = "" ] ; then
echo "Use: visit.sh <item.sh>"
exit
fi

item=$1
fitem=`readlink -f $item`

echo "visit: using $fitem"

die()
{
	echo "error: $1"
	exit
}

test -f $item || die "Cannot access $item"
test -f $fitem || die "Cannot access $fitem"

run()
{
  if cd "$1" ; then
  if test -z "$(ls -A)" ; then
  :
  else
    local i
    for i in *
    do
      #echo AAA $i $2 `pwd`
      test -d "$i" && run "$i" "$2/$i"
      #echo BBB $i $2 `pwd`
      bash $fitem "$i" "$2"
    done
  fi
  cd ..
  fi
}

run . .

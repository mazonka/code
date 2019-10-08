#!/bin/bash

# This script visits all directories and
# find empty directories

run()
{
  if cd "$1" ; then
  if test -z "$(ls -A)" ; then
  :
  else
    local i
    for i in *
    do
      test -d "$i" && run "$i" "$2/$i"
    done
  fi

  c=no
  test -z "$(ls -A)" && c=empty
  cd ..

    if [ "$c" = "empty" ] ; then
      echo "$1 is empty in $2"
      echo "REMOVING [$1]"
      rmdir "$1"
    fi

  fi
}

run . .

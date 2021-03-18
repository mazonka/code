#!/bin/bash

threshold=10000
busysleep=3
jobsleep=10
#busysleep=1
#jobsleep=1
gdrv="G:\My Drive\online\_in"
#gdrv="C:/0"


shopt -s nullglob

job()
{
speed=`bash netactivity.sh`
if [ "$speed" -gt "$threshold" ] ; then
echo "network busy ($speed) - sleep $busysleep seconds"
sleep $busysleep
else
echo "network not busy - moving file ($1) and sleep $jobsleep"

dirn=$(dirname -- "$1")
gpath="$gdrv/$dirn"
mkdir -p "$gpath/"
mv "$1" "$gpath/" 
sleep $jobsleep
fi
}


getfile()
{
file=
local i
for i in "$1"/*
do
if [ "$i" = "./copy2gdrive.sh" ] ; then
:
elif [ "$i" = "./netactivity.sh" ] ; then
:
else
  if test -f "$i" ; then
  file="$i"
  return
  fi

  if test -d "$i" ; then
  getfile "$i"

  if [ "$file" = "" ] ; then
    file=$i
  fi

  return
  fi

fi
done
}


getfile .
while [ "$file" != "" ]
do
echo "file to job: $file"

if test -d "$file" ; then
rmdir "$file"
else
job "$file"
fi

getfile .
done


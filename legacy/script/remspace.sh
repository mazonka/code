#!/bin/sh

PAT1=Feynman
PAT2=F

for i in *
do

OUT=$(echo $i | sed 's/ /_/g')
IN=$(echo $i | sed 's/ /\\ /g')

if [ "$i" == "$OUT" ]
then
 echo ignore "$i"
else
 echo "$i -> $OUT"

 cmd="svn mv $IN $OUT"
 echo $cmd
 svn mv "$i" $OUT

fi


done
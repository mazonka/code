#!/bin/sh

cd $1 || exit

PAT1=Feynman
PAT2=F

for i in *
do

OUT=$(echo $i | sed 's/FFeynman__-_/Joking/g')
OUT=$2_$i

if [ "$i" == "$OUT" ]
then
 echo ignore "$i"
else
 echo "$i -> $OUT"

 cmd="mv $i $OUT"
 echo $cmd
 $cmd

fi


done
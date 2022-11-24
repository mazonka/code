#!/bin/sh

for i in *
do

OUT=$(echo $i | sed 's/the age of empathy //g')

if [ "$i" == "$OUT" ]
then
 echo ignore "$i"
else
 echo "$i -> $OUT"

 mv "$i" "$OUT"

fi


done
#!/bin/bash

for i in {260..263}
do
echo $i

wget https://language-efficiency.com/$i -O $i.htm
#cat $i.htm | grep "yadi.sk" > $i.yad
./down_oscar.exe $i.htm
rm $i.htm


done

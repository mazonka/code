#!/bin/sh

base="10.1.1."

ctr="0"
: > ipsearch.out

while [ "$ctr" != "254" ]
do
ctr=$(($ctr+1))
echo -n "$base$ctr"
#ping -n 1 $base$ctr
#rpl=`ping -n 1 $base$ctr | grep '(0% loss)'`
rpl=`ping -n 1 $base$ctr | grep 'TTL'`
 if [ "$rpl" = "" ]; then
 echo ""
 else
 echo $base$ctr >> ipsearch.out
 echo " OK"
 fi
done


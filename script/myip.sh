#!/bin/bash

# this script logs public IP

M=5000

for (( i=1; i<$M; i++ ))
do
echo "$i/$M"
echo -n `date +%y%m%d%H%M%S` " " >> myip.log
curl https://api.ipify.org 2>&1 >> myip.log
echo "" >> myip.log
tail myip.log
sleep 10000
done

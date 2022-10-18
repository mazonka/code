#!/bin/bash

delay=3
#delay=1
threshold=10000



bytes=0
getbytes()
{
line=`netstat -e | grep Bytes`
aline=($line)
sent=${aline[1]}
recv=${aline[2]}
bytes=$((sent+recv))
}

diff=0
getdiff()
{
getbytes
b1=$bytes
sleep $1
getbytes
b2=$bytes
diff=$((b2-b1))
}

speed=0
getspeed()
{
getdiff $1
speed=$(( diff / $1 ))
}

getspeed $delay

echo $speed

#if [ "$speed" -gt "$threshold" ] ; then
#echo yes
#else
#echo no
#fi





#!/bin/sh

sc=10 # reduce 5x

ff="c:/app/FormatFactory/ffmpeg"

if [ "$1" = "" ]
then
echo use name
exit
fi

out=$2

if [ "$2" = "" ]
then
out=x_$1
fi

$ff -i $1 -vf scale=trunc\(iw/$sc\)*2:trunc\(ih/$sc\)*2 -c:v libx265 -crf 28 $out

#extract mp3 audio on 3rd channel
#C=3
#$ff -i $1 -vn -q:a 0 -map 0:$C $out.$C.mp3

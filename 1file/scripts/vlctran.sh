#!/bin/sh

scale=,scale=0.5
stop=--stop-time=10
#stop="--start-time=10 --stop-time=20"
#noface="-I dummy"

vlc="c:/Program Files (x86)/VideoLAN/VLC/vlc.exe"


vlctran()
{

file=$1

if test -f "$file"; then

#audio=,acodec=mp4a,ab=192,channels=2
#subt=,scodec=dvbsub,senc=dvbsub
#vbr=,vb=1024
#fps=,fps=24
deint=,deinterlace
sync=,audio-sync
codec=h264
#codec=mp4v

tran1=vcodec=$codec$vbr$fps$deint$scale$audio$subt$sync
tran2=mux=mp4,dst=vlctran_$$.avi,access=file

tran=--sout=#transcode{$tran1}:std{$tran2}

cmd="$noface -vvv $stop $tran vlc://quit"

echo "$vlc" "$file" $cmd
"$vlc" "$file" $cmd

mv vlctran_$$.avi "$file".avi

else

echo ERROR: no file "$file"

fi

}


if [ "$1" = "" ]
then

if test -f vlctran.lst
then

while read LINE
do
        echo ""
        echo "$LINE"
        vlctran "$LINE"
done < vlctran.lst

else
	echo Usage: sh vlctran.sh file.avi - convert one file
	echo Usage: sh vlctran.sh - convert all files in list vlctran.lst
fi

else

vlctran "$1"

fi


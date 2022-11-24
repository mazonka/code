#!/bin/bash

cwd=`pwd`
tm=/tmp${cwd// /_}

if test -d $tm; then
:
else
mkdir -p $tm
fi

cppname=$1
name=${1%.*}
exename=$name.exe
makname=$name.mak
fullmakname=$tm/$makname
wfullexename=`cygpath -m $tm/$exename`
wfullcppname=`cygpath -m "$cwd/$cppname"`

#echo "cppname=$1 name=$name"
echo "makname=$makname fullmakname=$fullmakname"
#echo "wfullexename=$wfullexename"
#echo "wfullcppname=$wfullcppname"

cwdcppname=$cwd/$cppname
cwdcppname=${cwdcppname// /\\ }
#echo "cwdcppname=$cwdcppname"
#exit

if test -f $fullmakname; then
:
else
echo creating $makname
echo "$tm/$exename: $cwdcppname" > $fullmakname
echo "	@rm -f $tm/$exename" >> $fullmakname
echo "	cl -EHsc -Ox -nologo \"$wfullcppname\" -Fe$wfullexename" >> $fullmakname
echo "	@rm -f $name.obj" >> $fullmakname
fi

make -f $fullmakname | grep -v "is up to date" 2> /dev/null

shift
#echo $tm/$exename $*
$tm/$exename $*






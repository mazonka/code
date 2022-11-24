#!/bin/sh

cwd=`pwd`
tm=/tmp$cwd

if [ "$1" = "clean" ]; then
rm -rf $tm
exit
fi


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
wfullcppname=`cygpath -m $cwd/$cppname`

if test -f $fullmakname; then
:
else
echo creating $makname
echo "$tm/$exename: $cwd/$cppname" > $fullmakname
echo "	@rm -f $tm/$exename" >> $fullmakname
echo "	cl -EHsc -Ox -nologo $wfullcppname -Fe$wfullexename" >> $fullmakname
echo "	@rm -f $name.obj" >> $fullmakname
fi

make -f $fullmakname | grep -v "is up to date" 2> /dev/null

shift
#echo $tm/$exename $*
$tm/$exename $*






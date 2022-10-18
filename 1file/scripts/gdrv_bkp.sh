#!/bin/bash

echo Backup google drive
bdir=c:/user/gdrive_bkp
gdrv=g:

datadir="$gdrv/My Drive/data"
momadir="$gdrv/.shortcut-targets-by-id/1V33oWAsIkttKlFFxGFi-LzZA1hepob9X/MoMADocs"

if test -d "$datadir" ; then
echo Data dir: $datadir
else
echo No data dir: $datadir
echo Create: $datadir/_samr
exit
fi

if test -d "$momadir" ; then
echo Data dir: $momadir
else
echo No data dir: $momadir
exit
fi


dt=`date +%y%m%d`

if test -d $bdir ; then
dtd=$bdir/$dt
echo Backup directory is $dtd
else
echo No backup directory $bdir
exit
fi

mkdir -p $dtd

echo copying data
cp -R "$datadir" $dtd/
echo copying moma
cp -R "$momadir" $dtd/


echo chmod 0777 -R $dtd
chmod 0777 -R $dtd

cd $dtd
echo Entering `pwd`

if test -d "../_samr" ; then
:
else
echo "No _samr: no sam ci done"
exit
fi

echo sam cir
sam cir

cd ..
fcl make -D $dt $dt
bzip2 -v $dt.fcl
chmod 0777 $dt.fcl.bz2

if test -f $dt.fcl.bz2 ; then
:
else
echo "Failed to make $dt.fcl.bz2"
exit
fi

rm -rf $dt.fcl $dt/

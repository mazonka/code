#!/bin/sh

svn=git

cmnd=${1:-up}
path=$2
cpwd=`pwd`
hpwd=

die(){ echo $1; exit; }

#search for hideg
xpwo=
while :
do
#echo "AAA cheking `pwd`"
if [[ -f "hideg" || -f "hideg.exe" ]]; then
   hpwd=`pwd`
   test -f "hideg" && hideg="hideg"
   test -f "hideg.exe" && hideg="hideg.exe"
   #echo "AAA hideg found at $hpwd"
   break
fi
cd ..
xpwd=`pwd`
if [ "$xpwd" = "$xpwo" ]; then
   die "Cannot find hideg"
fi
xpwo=$xpwd
done

./$hideg

cd $cpwd
#echo "AAA back to `pwd`"

#echo "AAA 1=$1 path=$path cpwd=$cpwd hideg=$hideg"

if [ -z "$path" ]; then
rpwd1=`realpath --relative-to="$cpwd" "$hpwd"`
path=`realpath --relative-to="$hpwd" "$cpwd"`
#echo "AAA rpwd1=$rpwd1 path=$path"
fi

cd $hpwd
#echo "We are at `pwd`"
#echo "hideg=$hideg path=$path"



up_GBT()
{
rel=$1
file=$2.g
fnog=$2

pushd $hpwd > /dev/null
echo -n "restore "
#echo "AAAghg ./$hideg $rel/_g/$file > /dev/null"
./$hideg $rel/_g/$file > /dev/null
mv $rel/_g/$fnog $rel/$fnog
touch $rel/_g/t/$fnog

md=`md5sum $rel/$fnog`
echo "[${md:0:6}] $rel/$fnog"

popd > /dev/null
}



up_chkin()
{
rel=$1
n=$2
#echo "AAA up_chkin relpath=$rel file=$n"

pushd $hpwd > /dev/null
echo -n "checkin "
md=`md5sum $rel/$n`
echo -n "[${md:0:6}] "
./$hideg $rel/$n
mv -f $rel/$n.g $rel/_g/$n.g
rm -f $rel/_g/t/$n $rel/$n
popd > /dev/null

up_GBT $1 $2
}



up_proc_files()
{
cwd=`pwd`
cd _g 2> /dev/null || return
mkdir -p t

rel=`realpath --relative-to="$hpwd" "$cwd"`

for i in *.g
do
   n=${i%.*}
   if [[ ! -f ../$n || ! -f t/$n || ( $i -nt ../$n ) ]]; then
   #echo "AAA process $i"
   up_GBT $rel $n
   elif [[ ../$n -nt t/$n ]]; then
   #echo "AAA process $i"
   up_chkin $rel $n
   else
:
#   echo "$rel/$n - OK"
:
   fi 
done
cd ..
}



up_fun()
{
cd $1
up_proc_files
for i in *
do
   if [ -d $i ];then
      if [ $i = "_g" ];then
      :
      else
      up_fun $i
      fi
   fi
done
cd ..
}



clean_proc_files()
{
cwd=`pwd`
cd _g 2> /dev/null || return
rm -rf t

rel=`realpath --relative-to="$hpwd" "$cwd"`

for i in *.g
do
   n=${i%.*}
   #echo "AAA g-clean $i $n $rel"
   rm -f ../$n 
done
cd ..
}



clean_fun()
{
cd $1
clean_proc_files
for i in *
do
   if [ -d $i ];then
      if [ $i = "_g" ];then
      :
      else
      clean_fun $i
      fi
   fi
done
cd ..
}



add_fun()
{
cwd=`pwd`
rel=`realpath --relative-to="$hpwd" "$cwd"`
n=$path

pushd $hpwd > /dev/null
echo -n "adding "
md=`md5sum $rel/$n`
echo "[${md:0:6}] $n"
./$hideg $rel/$n

if test -d $rel/_g; then
   mv -f $rel/$n.g $rel/_g/$n.g
   rm -f $rel/_g/t/$n
   svncmd="$svn add $rel/_g/$n.g"
   echo $svncmd; $svncmd
else
   mkdir -p $rel/_g
   mv -f $rel/$n.g $rel/_g/$n.g
   svncmd="$svn add $rel/_g"
   echo $svncmd; $svncmd
fi
popd > /dev/null

mkdir -p _g/t
mv $n $n.bkp.tmp
up_GBT $rel $n
if cmp $n $n.bkp.tmp; then
rm -f $n.bkp.tmp
else
echo "Error while converting file, recover from .bkp.tmp"
fi
}



remove_fun()
{
cwd=`pwd`
rel=`realpath --relative-to="$hpwd" "$cwd"`
n=$path

pushd $hpwd > /dev/null
echo -n "removing $n"
svncmd="$svn rm $rel/_g/$n.g"
echo $svncmd; $svncmd
rm -f $rel/$n $rel/_g/t/$n
popd > /dev/null
}




if [ "$cmnd" = "up" ]; then
	cd $path 2> /dev/null || die "Cannot enter $path"
	cd $hpwd
	up_fun $path
	exit

elif [ "$cmnd" = "clean" ]; then
	cd $path 2> /dev/null || die "Cannot enter $path"
	cd $hpwd
	clean_fun $path
	exit

elif [ "$cmnd" = "add" ]; then
	cd $cpwd
	test -f $path 2> /dev/null || die "File not found $path"
	add_fun $path
	exit

elif [ "$cmnd" = "remove" ]; then
	cd $cpwd
	test -f $path 2> /dev/null || die "File not found $path"
	remove_fun $path
	exit
fi

echo "Incorrect cammand $cmnd"
echo "Usage (up|clean) [path] - synch unencrypted files with .g file"
echo "Usage (add|remove) file - this use svn command to add and remove .g files"
exit


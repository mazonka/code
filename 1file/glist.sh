#!/bin/bash


pwd=mazonka:
version=2010.2

list=glist.sh

cwd=`pwd`

if test -f $list; then
:
else
echo "no file $list"
exit
fi

if test -f token.bzc ; then
gf -k -s unpack token.bzc
else
echo "no token file"
exit
fi

#printf '%b' $(printf '\\x%x' {65..122})
#pqstr=`printf '%b' $(printf '\\\\x%x' {91,92,111,43,110,53,53,51,55,53,64})`
pqstr=`cat token`@
pwd=$pwd$pqstr
pwd=$pqstr

rm token

#echo $pwd
#exit

pwdsz=${#pwd}
if [ "$pwdsz" -lt "10" ]; then
echo "Fix pwd"
exit
fi

#echo "Pword $pwdsz [$pwd]"
#exit

my_echo()
{
echo my_echo [$1] [$2] [$3] [$4]
}

my_list()
{
echo "$1/$2 ($3/$4)"
}

dirstd_list()
{
if [ "$3" = "." ] ; then
echo "$4"
else
echo "$3/$4"
fi
}

dircur_list()
{
pwd
#echo "$3/$4"
}

ping()
{
echo "$1/$2"
git ls-remote https://${pwd}github.com/$1/$2 > /dev/null
}

clone()
{
echo "$0: clone - $1/$2 ($3/$4)"
mkdir -p $3
pushd $3 > /dev/null
#pwd

if test -d $4; then
echo "$0: Directory exists - skip"
else
git clone --depth 1 https://${pwd}github.com/$1/$2 $4
rm -rf $4/.git/logs

echo "git pull" > $4/g_pull.bat
echo ":: pull hard" >> $4/g_pull.bat
echo ":: xgit fetch --all" >> $4/g_pull.bat
echo ":: xgit reset --hard origin/master" >> $4/g_pull.bat

echo "git commit -a -m z" > $4/g_push.bat
echo "git push" >> $4/g_push.bat
echo "::git ls-files -m -o" >> $4/g_push.bat

chmod 0777 $4/g_push.bat $4/g_pull.bat

fi

popd > /dev/null
}

loop(){
startlist=

while read -r LINE; do
if [ "$LINE" = "" ]; then
:
else
  if [ "$startlist" = "" ]; then
    if [ "$LINE" == "#startlist" ]; then
      startlist=ok
    fi
  else
    char=${LINE:0:1}
    if [ "$char" = "#" ]; then
    :
    else
    varr=($LINE)
    var1=${varr[0]}
    var2=${varr[1]}
    var3=${varr[2]}
    var4=${varr[3]}

    if [ "$var3" = "" ]; then var3=$var1; fi
    if [ "$var4" = "" ]; then var4=$var2; fi
    if [ "$var1" = "exit" ]; then exit; fi

    #echo "AAA $1 [$var1] [$var2] [$var3] [$var4]"
    $1 $var1 $var2 $var3 $var4
    fi
  fi
fi
done < $list
}
# loop

fixgit()
{
pwd
git config --local core.filemode false
git config pull.rebase false
}

pull()
{
pwd
git pull
}

status()
{
pwd
git status . -s
}

stat()
{
pwd
git status . -s | grep -v "??"
}

thisdir()
{
  pushd $1 > /dev/null
  $2
  popd > /dev/null
}

alldir()
{
for i in *
do
if test -d $i ; then
if test -d $i/.git ; then
#  pushd $i > /dev/null
#  $1
#  popd > /dev/null
  thisdir $i $1
else

  for j in $i/*
  do
  if test -d $j ; then
  if test -d $j/.git ; then
    thisdir $j $1
  fi
  fi
  done

fi
fi
done
}
# alldir


extra(){
loop dirstd_list | sort > glist.std.tmp
alldir dircur_list | sort | sed -e "s|$cwd/||g" > glist.cur.tmp
comm -$1 glist.cur.tmp glist.std.tmp
rm -f glist.*.tmp
}
# extra


#loop my_echo

if [ "$1" = "" ] ; then 
echo "glish.sh version $version"
echo "Use commands: list, clone, ping, pull, status, stat, extra, missing, fix"

elif [ "$1" = "list" ]; then
loop my_list

elif [ "$1" = "ping" ]; then
loop ping

elif [ "$1" = "clone" ]; then
loop clone

elif [ "$1" = "pull" ]; then
alldir pull

elif [ "$1" = "status" ]; then
alldir status

elif [ "$1" = "stat" ]; then
alldir stat

elif [ "$1" = "extra" ]; then
extra 23

elif [ "$1" = "missing" ]; then
extra 13

elif [ "$1" = "fix" ]; then
alldir fixgit

else
echo "$1 is not a command"
fi


exit










####################################################
#
# gituser project dir[=gituser] name[=project]
#startlist

mazonka code
mazonka driller
mazonka godaddy
mazonka hw2 .
mazonka w .
mazonka artill
mazonka jraf
mazonka phenomainz
mazonka mozaika
mazonka tools .
mazonka apg

hasq dev
hasq bin

#momalab moma-interns-2020
momalab simfhe .
momalab e3 .
#momalab e3.wiki .
momalab e3work .
#momalab e3circuit
#momalab idash
#momalab e3store
#momalab CoPHEE
#momalab cofhe
#momalab e3genome
#momalab TERMinatorSuite
momalab cryptoleq
momalab cealdemo
momalab cealbook

#essa2307 DARPA_MNIST
#deepraj88 NTT

#!/sh/bin

file=_md5valid.txt

cwd=`pwd`

function clean_rec
{
pushd "$1" > /dev/null

for i in *
do
if test -d "$i"; then
clean_rec "$i"
fi
done

echo cleaning `pwd`
rm -f $file

popd > /dev/null
}

function clean
{
echo clean
clean_rec .
}

function build_rec
{
pushd "$1" > /dev/null

echo "( `pwd` )"

for i in *
do
if test -d "$i"; then
build_rec "$i"
fi
done

if test -f $file; then
:
else

for i in *
do
if [ "$i" = "*" ]
then
:
elif test -d "$i"; then
echo "[ `pwd` $i ]"
#md5sum "$i/$file" >> $file
else
echo $i
md5sum "$i" >> $file
fi

done

fi
popd > /dev/null
}

function build
{
build_rec .
}

function valid_rec
{
pushd "$1" > /dev/null

for i in *
do
if test -d "$i"; then
valid_rec "$i"
fi
done

if test -f $file
then
if md5sum -c --status $file; then
echo "`pwd` [OK]" | tee -a "$cwd/_md5good.log"
else
echo FAIL `pwd` | tee -a "$cwd/_md5failed.log"
fi
fi

popd > /dev/null
}

function valid
{
valid_rec .
}

if [ "$1" = "build" ]
then
build
exit
fi

if [ "$1" = "valid" ]
then
valid
exit
fi

if [ "$1" = "clean" ]
then
clean
rm -f md5good.log md5failed.log
exit
fi

echo "Usage: sh md5valid.sh { biuld | valid | clean }"


#!/sh/bin

ndir="$1"

cwd=`pwd`

function build_rec
{
pushd "$1" > /dev/null

echo "( $2 )"
mkdir -p "$ndir/$2"

for i in * .*
do
if [ "$i" = "." ]
then
:
elif [ "$i" = ".." ]
then
:
elif test -d "$i"; then
build_rec "$i" "$2/$i"
fi
done

## do not touch files
# for i in * .*
# do
# if [ "$i" = "*" ]
# then
# :
# elif test -d "$i"; then
# :
# else
# echo "{ $2/$i }"
# touch "$ndir/$2/$i"
# fi
# done

popd > /dev/null
}


if [ "$1" = "" ]
then 
echo "copytree.sh copies current directory structure into another"
echo "Usage: sh copytree.sh <dir>"
exit
fi

if test -d "$1"; then

pushd "$1" > /dev/null
ndir=`pwd`
popd > /dev/null

echo "`pwd` -> $ndir"

build_rec . .

else

echo No access to directory "$1"

fi


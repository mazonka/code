#!/bin/sh

if [ "$1" = "" ]; then
echo This script embeds md5 sum into file\'s name or checks the sum if embedded
echo Usage: md5ing.sh filename
exit
fi

code=x
function md5code
{
#md5sum $1 > $1.md5
#while read md line; do
#code=$md
#done < $1.md5
#rm $1.md5
v=`md5sum $1`
code=${v%%\ *}
}

function to_md5
{
md5code $1
mv $1 $1.md5ed.$code
}


function test_md5
{
cde=${1##*.}
md5code $1
if [ "$cde" = "$code" ]; then
echo tested OK
else
echo ERROR: MD5 test FAILED
echo given code $cde
echo calculated $code
fi
}


if echo "$1" | grep ".md5ed." > /dev/null ; then
test_md5 $1
else
to_md5 $1
fi

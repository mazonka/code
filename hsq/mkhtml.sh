#!/bin/sh

cwd=`pwd`
testdir=tests
mkdir -p $testdir
htfile=$cwd/$testdir/index.html

fmk(){

echo making $1 ...

out=$cwd/$testdir/$1.html
echo " &nbsp; &nbsp; <a href='$1.html'>$1</a>" >> $htfile

test -f $out && echo "ERROR file $out present" && exit

$cwd/hsq.exe -a $1.hsq 2> $1.err
$cwd/hsq.exe -d $1.hsq
test -f $1.asq && $cwd/hsq.exe $1.asq > $1.out

echo "<html><body bgcolor="#bbbbbb" topmargin=20 leftmargin=20 rightmargin=20>" > $out

echo "<h2>Test: $1</h2>" >> $out
echo "<table border=1 cellpadding=10>" >> $out
echo "<tr><th>Higher Subleq</th><th>Error</th><th>Output</th><th>Subleq</th><th>ITR</th></tr>" >> $out

echo "<td valign=top><pre>" >> $out
cat $1.hsq | sed -e 's%\t%   %g' | sed -e 's%<%\&lt;%g' >> $out
echo "</pre><hr width=90%></td>" >> $out

echo "<td valign=top><pre>" >> $out
cat $1.err | sed -e 's%\t%   %g' >> $out
echo "</pre><hr width=90%></td>" >> $out

echo "<td valign=top><pre>" >> $out
test -f $1.out && cat $1.out | sed -e 's%\t%   %g' >> $out
echo "</pre><hr width=90%></td>" >> $out

echo "<td valign=top><pre>" >> $out
test -f $1.asq && cat $1.asq | sed -e 's%\t%   %g' >> $out
echo "</pre><hr width=90%></td>" >> $out

echo "<td valign=top><pre>" >> $out
test -f $1.itr && cat $1.itr | sed -e 's%\t%   %g' >> $out
echo "</pre><hr width=90%></td>" >> $out

echo "</tr></table>" >> $out

echo "</body></html>" >> $out

rm -rf $1.out $1.asq $1.itr $1.err

}


frec(){
if test -d "$1"; then

   if test -h "$1"; then
	return
   fi
   if test -x "$1"; then
	:
   else
	return
   fi

   cd "$1"
   echo "enter $1"
   echo "<br> $1: " >> $htfile

   for i in *
   do
    frec "$i"
   done

   if ls *.hsq 1> /dev/null 2> /dev/null ; then
   for i in *.hsq
   do
    echo `basename "$i" .hsq`
    fmk `basename "$i" .hsq`
   done
   fi

   echo "exit $1"
   cd ..
fi
}

echo "<html>" > $htfile
echo "generated" `date` "<br>" >> $htfile
frec sts
echo "</html>" >> $htfile


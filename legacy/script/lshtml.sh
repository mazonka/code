#!/bin/bash
#!/bin/ksh
#
# ls2html is a k-shell script to create a directory listing in HTML format
# by modest re-formatting of a UNIX 'ls' command.
# 
# usage:
#   ls2html -r [dir1 ...] > fools-lR.html
#   ls2html [dir1 ] > fooindex.html
#
# If the option -h is not used, one gets a simple, non-clickable HTML
# page on standard output, which shows an expanded UNIX directory listing
# of the directories and, recursively, all their subdirectories.
#
# If the option -h is used, the output contains a clickable directory
# listing of the single named directory, excluding any file named "index.html"
# This is probably to be called from another script that recursively
# creates index files like this one:
##!/bin/ksh
##
## ls2index is a k-shell script to create index.html files in the subdirectories
## by recursive calls of ls2html .
## 
## usage:
##   ls2index dir1 [dir2 ...]
##
## Richard J Mathar, 11/04/2003
#
## Remember the current working directory in case that the application
## uses relative path names in the directory names (=the argument list)
#startdir=`pwd`
#dirli=`find $* -type d -print`
#
## echo $dirli
#
#for dir in $dirli  ; do
#        # go to the specific directory; interpret file names as
#        # absolute file names if they start with the wiggle or a slash
#        if [[ $dir = ~* || $dir = /* ]] ; then
#                cd $dir
#        else
#                cd $startdir/$dir
#        fi
#        # The full path name for the executable to allow working from crontab(1)
#        #~/bin/list-files.pl > index.html
#        ~/bin/ls2html -h > index.html
#        chmod a+r index.html
#done

#
# Richard J Mathar, 03/11/2001

function line2html {
	while { read dirent ; }
	do
		ww=`echo $dirent |wc -w` ;
		if [ $ww -eq 1 ]
		then
			echo "<font color=green>"$dirent"</font>"
		elif [ $ww -eq 0 ]
		then
			echo
		else
			if [ "$1" = -h ]
			then
				fordir=$(echo $dirent | cut -d ' ' -f 7)
				printf "%10s %2d %10d %3s %2d %5s <a href=\"%s\">%s</a>\n" $dirent $fordir;
			else
				# be prepared to write foo -> foo.link or filenames with blanks
				printf "%10s %2d %10d %3s %2d %5s %s %s %s\n" $dirent ;
			fi
		fi
	done
}

echo '<html>'
echo '<head>'
echo '<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">'
echo '<meta http-equiv="Content-Language" content="en">'
echo '<meta http-equiv="Last-Modified" content="'`date`'">'
echo '<meta name="author" content="'$0'">'
echo '<title>'
echo `uname -n` $*
echo '</title>'
echo '</head>'
echo '<body bgcolor="#f6feff">'
echo '<pre>'

if [ "$1" = -r ]
then
	ls -oglAR $*  | sed '/^total/d' | line2html 
else
	ls -aoglFL $2  | fgrep -v index.html | sed '/^total/d' | line2html -h
fi

echo '</pre>'
echo `date`
echo '</body>'
echo '</html>'

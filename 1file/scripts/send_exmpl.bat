::@echo off
:::::::::::::: Lets set some variables ::::::::::::::
set eMail=om@hasq.org
set subj=-s "Test Blat"
set server=-server 127.0.0.1
set x=-x "X-Header-Test: Can Blat do it? Yes it Can!"
set debug=-debug -log blat.log -timestamp 
set au=-u hasqsvn -pw Adelaide14
::::::::::::::::: Now we run Blat!  :::::::::::::::::
blat %0 -to %eMail% -f %eMail% %subj% %server% %debug% %x% %au%
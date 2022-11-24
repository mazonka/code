..\cyg\bin\echo -n "set APP=" > setapp.bat
..\cyg\bin\cygpath -wa .. >> setapp.bat
call setapp.bat
del setapp.bat

set PATH=%APP%\run;%APP%\svn;%APP%\cyg\bin;%APP%\run2;%windir%;%windir%\system32

set PATH=%APP%\c15\bin;%PATH%
set INCLUDE=%APP%\c15\include
set LIB=%APP%\c15\lib


call cygumnt.bat
call cygmnt.bat
call attr.bat

if exist %windir%\cyg\bin\cygwin1.dll goto cygw
if exist %windir%\system32\cyg\bin\cygwin1.dll goto cygw

goto start

:cygw
@echo ** WARNING ****************************************************
@echo * You have cygwin1.dll in %windir% or %windir%\system32 directory
@echo * This will potentially cause problem with gcc compiler
@echo * [You are still able to use other compilers]
@echo ***************************************************************
@pause

:start
cd ..\..


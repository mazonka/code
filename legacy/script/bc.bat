@set PATHO=%PATH%
@set PATH=%APP%\bcc\bin;%APP%\bcc;
@bcc32 -I"%APP%\Bcc\include" -L"%APP%\Bcc\lib" %*
@set PATH=%PATHO%

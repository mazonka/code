for /f "delims=" %%a in ('ls -l') do @set USR=%%a
echo AAA %USR% AAA
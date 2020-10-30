::call style.bat *.cpp *.h
::make c
make -j 4
copy sam.exe %APP%\app\bin\

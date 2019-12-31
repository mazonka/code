::call style.bat *.cpp *.h
make c
make -j 8
copy sam.exe %APP%\run\

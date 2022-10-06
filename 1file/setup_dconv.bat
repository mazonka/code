cl -Ox -EHsc -std:c++20 dconv.cpp
rm dconv.obj
rm -f c:/apg/app/bin/dconv.exe
mv dconv.exe c:/apg/app/bin/

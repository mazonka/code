fcl3 make -D abc abc.fcl

::sh -c "echo \"/* `cat abc.fcl` */\"" > abc.cpp

echo /* > abc.cpp
cat abc.fcl >> abc.cpp
echo */ >> abc.cpp

rm -f abc.fcl
rm -f abc.exe

cl -EHsc abc.cpp && rm -f abc.cpp

rm -f abc.obj

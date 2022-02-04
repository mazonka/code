#!/bin/bash

# if [ "$#" = "0" ]; then
#	echo "at least one argument is required"
#	exit 1
# fi

for FILE in *.h
do

#FILE=$i
echo processing $FILE

GUARD="_E3_$(printf "%s" $FILE | tr '.' '_' | tr '[[:lower:]]' '[[:upper:]]')_"

mv $FILE $FILE.bak
echo "#ifndef $GUARD" > $FILE
echo "#define $GUARD" >> $FILE
cat $FILE.bak | grep -v "#pragma once" >> $FILE
echo "#endif // $GUARD" >> $FILE

# cat <<EOF >> $FILE
# #ifndef $GUARD
# #define $GUARD
# #endif // $GUARD
# EOF

done

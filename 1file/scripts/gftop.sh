#!/bin/bash

echo a

top=`gf v top`
dat=`date +%y%m%d%H%M%S`

echo $dat $top
echo $dat $top >> gftop


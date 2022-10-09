#!/bin/sh

# non-recursive renaming files to remove dots

for f in *; do mv "$f" "${f//\./_}"; done

#!/bin/sh

# non-recursive renaming files to remove spaces

for f in *\ *; do mv "$f" "${f// /_}"; done

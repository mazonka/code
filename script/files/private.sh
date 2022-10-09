#!/bin/sh

grep -rl "public:" . | xargs sed -i 's/private\:/public\:/g'
grep -rl "public:" . | xargs sed -i 's/protected\:/public\:/g'

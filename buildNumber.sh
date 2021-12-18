#!bin/bash

cat "$1" | sed "s/@BUILD_NUMBER@/$(git log -1 --format=%h)/" > "$2"

#!/bin/sh

cur=$(pwd)
nemu="$cur/ics2024/nemu"

format(){
   find "$nemu/src" -type f -name "*.c" > $cur/files
   find "$nemu/src" -type f -name "*.h" >> $cur/files
   clang-format -i --style=file:$cur/.clang-format --verbose --files=$cur/files
   rm $cur/files
}


if [ -f .clang-format ];then
    echo "Using \".clang-format\" to format files"
    format
else 
    echo "No \".clang-format\" file, exit"
    exit 0
fi

echo "ALL files formated."

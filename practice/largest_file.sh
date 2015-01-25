#! /bin/bash

PATH=/bin:/usr/bin

if test $# -eq 0
then
    echo usage: $0 file ... >&2
    exit 1
fi

maxsize=-1
for i
do
    echo $i
    thissize=`wc -c <"$i"`
    if test $thissize -gt $maxsize
    then
        maxsize=$thissize
        maxname="$i"
    fi
done

echo "$maxname"

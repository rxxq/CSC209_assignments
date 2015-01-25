#! /bin/bash

for i in "$*"; do echo $i; done
for i in "$@"; do echo $i; done

for i in $*; do echo $i; done
for i in $@; do echo $i; done

set `date`
echo $*
while test $# -gt 0
do
    shift
    echo $*
done

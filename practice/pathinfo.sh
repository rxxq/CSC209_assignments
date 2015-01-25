# /bin/bash

path=$1

if test ! -d $path
then
    echo 'invalid path!'
    exit 1
fi

if test `dirname $path` = '.' -o `dirname $path` = /
then
    exit 0
fi

subdir_count=0
tot_size_of_files=0

for file in $path/*
do
    if test -f $file
    then
        file_size=`du $file | cut -f 1`
        tot_size_of_files=`expr $tot_size_of_files + $file_size`
    fi
    if test -d $file
    then
        subdir_count=`expr $subdir_count + 1`
    fi
done

echo $path $subdir_count $tot_size_of_files

path=`dirname $path`
./pathinfo.sh $path

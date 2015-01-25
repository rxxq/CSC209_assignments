#! /bin/bash

function foo {
    for i in $*
    do
        echo $i
    done
}

foo a string with spaces

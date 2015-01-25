#! /bin/bash

read word < $1; for i in $word; do echo $i; exit; done

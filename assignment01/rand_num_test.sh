#! /bin/bash

count=0
while test $count -lt 1000
do
	rand_num=`expr \`expr $RANDOM / 512\` + 1`
	
	if test $rand_num -lt 1 -o $rand_num -gt 64
	then 
		echo "error $rand_num"
	#else 
		#echo "no error $rand_num"
	fi
	
	count=`expr $count + 1`
done

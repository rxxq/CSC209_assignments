#! /bin/sh

# usage: token_match -n column filename

# check if a position was defined
pos=0
if [ $# -eq 3 ]
then
    pos=$2
    shift 
    shift
else
    # throw Usage message and exit
    echo "Usage $0 -n column filename" 
    exit 1
fi

filename=$1
echo "File to open is ${filename}"
echo "Token must be found in position ${pos} on a line"

while read line
do
    if [ "$line" ]  
        then
        # break the line into $1, $2, etc.
        set $line

	# print the first token on the line
	echo "First word: $1"
	
	# now print the n-th token on the line
	if [ $pos -lt $# ]
        then 
	    # get the token we want positioned as $1
            shift $pos
            
            # print token
            echo "Word in position ${pos}: $1"
        fi
    fi
done < $filename

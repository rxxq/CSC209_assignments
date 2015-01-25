#! /bin/bash

# CSC209 Assignment 1
# Written by Xiuqi (Rex) Xia

# Keep looping until the user chooses to exit
while test 1
do
	# Clear the screen and print the menu
	clear
	echo \
'----------------------------------------
M A I N - M E N U
----------------------------------------
1. Number of processors
2. Memory information
3. Number of users currently logged in
4. Users with bash shell
5. Exit
----------------------------------------'

	# Prompt the user for a choice
	# Using -n flag to suppress newline at the end
	echo -n 'Please enter option [1 - 5]: '

	# Read input and decide what to do
	read option
	case $option in
		# Start with a tab if the output is only a number
		# Count number of cores by grepping for blank lines in cpuinfo
		1) echo -n '	'
		   grep -c '^$' /proc/cpuinfo;;

		# Grep for these three lines in meminfo
		2) grep -e MemTotal -e MemFree -e Shmem /proc/meminfo;;

		# Use who, remove duplicates
		3) echo -n '	'
		   who | cut -d ' ' -f 1 | sort | uniq | wc -l;;

		# Grep for bash processors, remove duplicates and grep itself
		4) ps aux | grep bash | grep -v grep |
		   cut -d ' ' -f 1 | sort | uniq ;;

		5) exit;;

		# Anything else is invalid
		*) echo 'Invalid option!'
		   echo 'You must enter a number between 1 and 5 (inclusive)';;
	esac
	
	echo -n 'Press [enter] key to continue...'
	# The code won't go past this read command until the user
	# presses the enter key.
	# The -s flag stops the user's keypresses from showing up on screen
	read -s option

done

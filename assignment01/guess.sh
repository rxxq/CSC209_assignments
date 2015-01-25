#! /bin/bash

# CSC209 Assignment 1
# Written by Xiuqi (Rex) Xia


# Define a random integer for user to guess
# Need to convert the range of $RANDOM (0-32767) to (1-64)
# by dividing by 512 and adding 1
target=`expr \`expr $RANDOM / 512\` + 1`

# For debugging use only
#echo target: $target

# Print welcome message
echo 'Welcome to the number game.'
echo 'Guess a number between 1 and 64 (inclusive).'

# Keep looping while the user has not guessed 6 times
# and did not guess correctly yet
correct=0
guess_counter=0
while test $guess_counter -lt 6 -a $correct -ne 1
do
	# If this is not the first guess, prompt the user to try again
	if test $guess_counter -gt 0; then echo 'Try again.'; fi

	# Get input 
	read guess

	# Test if guess is correct
	if test $guess -eq $target
	then
		# Signal the loop to exit on next iteration
		correct=1 
	else	
	# If guess is wrong, tell the user why it is wrong

		# Guess out of range (1 to 64)
		if test $guess -lt 1 -o $guess -gt 64
		then 
			echo 'Guess must be between 1 and 64 (inclusive)'
		# Guess is within range, but greater than or less than target
		elif test $guess -lt $target
		then
			echo 'Too small.'
		else
			echo 'Too big.'
		fi

		# Increment counter
		guess_counter=`expr $guess_counter + 1`
	fi
done

# If the loop exited due to a correct guess
if test $correct -eq 1
then
	# Print win message
	echo 'You won!'
	echo "THE ANSWER IS $target."
else
	# Print lose message
	echo 'You lost!'
	echo "THE ANSWER WAS $target."
fi

exit

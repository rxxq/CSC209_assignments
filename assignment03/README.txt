Name: Xiuqi Xia
Student number: 998269449
CDF login: c4xiaxiu
Grace days used: 2

I modified parser.c to check the first token and set the builtin field
in simple command to BUILTIN_CD for "cd" and BUILTIN_EXIT for "exit"
(instead of just setting it to 1 in both cases, as the starter code
originally did)

I was not able to make the cd command work properly in a complex command.
Since the execute_complex_command always forks off a child process for
each command it executes, the cd in a complex command is unable to change
the working directory of the original shell process.
I tested this in bash, and it seems like a cd in a piped context dosen't
work in bash either.

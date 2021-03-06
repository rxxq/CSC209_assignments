#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <mcheck.h>

#include "parser.h"
#include "shell.h"

/**
 * Program that simulates a simple shell.
 * The shell covers basic commands, including builtin commands (cd and exit only), 
 * standard I/O redirection and piping (|). 
 * 
 * You should not have to worry about more complex operators such as "&&", ";", etc.
 * Your program does not have to address environment variable substitution (e.g., $HOME), 
 * double quotes, single quotes, or back quotes.
 */

#define MAX_DIRNAME 100
#define MAX_COMMAND 1024
#define MAX_TOKEN 128

/* Functions to implement, see below after main */
int execute_cd(char** words);
int execute_nonbuiltin(simple_command *s);
int execute_simple_command(simple_command *cmd);
int execute_complex_command(command *cmd);


int main(int argc, char** argv) {
	
	char cwd[MAX_DIRNAME];           /* Current working directory */
	char command_line[MAX_COMMAND];  /* The command */
	char *tokens[MAX_TOKEN];         /* Command tokens (program name, parameters, pipe, etc.) */

	while (1) {

		/* Display prompt */		
		getcwd(cwd, MAX_DIRNAME-1);
		printf("%s> ", cwd);
		
		/* Read the command line */
		gets(command_line);
		
		/* Parse the command into tokens */
		parse_line(command_line, tokens);

		/* Empty command */
		if (!(*tokens))
			continue;
		
		/* Exit */
		if (strcmp(tokens[0], "exit") == 0)
			exit(0);
				
		/* Construct chain of commands, if multiple commands */
		command *cmd = construct_command(tokens);
		//print_command(cmd, 0);
    
		int exitcode = 0;
		if (cmd->scmd) {
			exitcode = execute_simple_command(cmd->scmd);
			if (exitcode == -1)
				break;
		}
		else {
			exitcode = execute_complex_command(cmd);
			if (exitcode == -1)
				break;
		}
		release_command(cmd);
	}
    
	return 0;
}


/**
 * Changes directory to a path specified in the words argument;
 * For example: words[0] = "cd"
 *              words[1] = "csc209/assignment2/"
 * Your command should handle both relative paths to the current 
 * working directory, and absolute paths relative to root,
 * e.g., relative path:  cd csc209/assignment2/
 *       absolute path:  cd /u/bogdan/csc209/assignment2/
 * You don't have to handle paths containing "~" or environment 
 * variables like $HOME, etc.. 
 */
int execute_cd(char** words) {
	
    //fprintf(stderr, "execute cd\n");


	/** 
	 * DONE:
	 * The first word contains the "cd" string, the second one contains the path.
	 * Check possible errors:
	 * - The words pointer could be NULL, the first string or the second string
	 *   could be NULL, or the first string is not a cd command
	 * - If so, return an EXIT_FAILURE status to indicate something is wrong.
	 */
    if(!(*words)){
        fprintf(stderr, "cd error: first token is NULL\n");
        return EXIT_FAILURE;
    }
    if(strcmp(*words, "cd")){
        fprintf(stderr, "cd error: first token is not \"cd\"\n");
        return EXIT_FAILURE;
    }
    if(!(*(words+1))){
        fprintf(stderr, "ed error: second token is NULL\n");
        return EXIT_FAILURE;
    }


	/**
	 * DONE: 
	 * The safest way would be to first determine if the path is relative 
	 * or absolute (see is_relative function provided).
	 * - If it's not relative, then simply change the directory to the path 
	 * specified in the second word in the array.
	 * - If it's relative, then make sure to get the current working directory, 
	 * append the path in the second word to the current working directory and 
	 * change the directory to this path.
	 * Hints: see chdir and getcwd man pages.
	 * Return the success/error code obtained when changing the directory.
	 */

    // chdir can already handle both relative and absolute addresses
	if(chdir(*(words+1)) == -1){
        perror("chdir");
        return 1;
    }
    return 0;
}


/**
 * Executes a regular command, based on the tokens provided as 
 * an argument.
 * For example, "ls -l" is represented in the tokens array by 
 * 2 strings "ls" and "-l", followed by a NULL token.
 * The command "ls �l | wc -l" will contain 5 tokens, 
 * followed by a NULL token. 
 */
int execute_command(char **tokens) {
	
    //fprintf(stderr, "Execute command: %s\n", tokens[0]);
	//fprintf(stderr, "%s\n", tokens[1]);
    
    /**
	 * DONE: execute a regular command, based on the tokens provided.
	 * The first token is the command name, the rest are the arguments 
	 * for the command. 
	 * Hint: see execlp/execvp man pages.
	 * 
	 * - In case of error, make sure to use "perror" to indicate the name
	 *   of the command that failed.
	 *   You do NOT have to print an identical error message to what would happen in bash.
	 *   If you use perror, an output like: 
	 *      my_silly_command: No such file of directory 
	 *   would suffice.
	 * - Function returns the result of the execution.
	 */
    if(execvp(tokens[0], tokens) == -1){
        perror(tokens[0]);
        return 1;
    }

    // code should never reach here
    return -1;
}


/**
 * Executes a non-builtin command.
 */
int execute_nonbuiltin(simple_command *s)
{
    //fprintf(stderr, "Execute non-builtin: %s\n", s->tokens[0]);
	
    /**
	 * DONE: Check if the in, out, and err fields are set (not NULL),
	 * and, IN EACH CASE:
	 * - Open a new file descriptor (make sure you have the correct flags,
	 *   and permissions);
	 * - Set the file descriptor flags to 1 using fcntl - to avoid leaving 
	 *   the file descriptor open across an execve. Don't worry about this
	 *   too much, just use this: 
	 *         fcntl(myfd, F_SETFD, 1);
	 *   where myfd is the file descriptor you just opened.
	 * - redirect stdin/stdout/stderr to the corresponding file.
	 *   (hint: see dup2 man pages).
	 */
    int fd;

	// stdin from file
    if(s->in){
        // Open in read-only mode.
        if((fd = open(s->in, O_RDONLY)) == -1){
            perror("open s->in");
            return 1;
        }
        fcntl(fd, F_SETFD, 1);
        dup2(fd, fileno(stdin));
        if(close(fd) == -1){
            perror("close fd");
            return 1;
        }
    }
    // stdout to file
    if(s->out){
        // Open in write-only mode. If file does not exist create it.
        // If file already exists truncate it to zero.
        // If new file is created, set permissions to
        // allow read, write, exec by user.
        if((fd = open(s->out, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU)) == -1){
            perror("open s->out");
            return 1;
        }
        fcntl(fd, F_SETFD, 1);
        dup2(fd, fileno(stdout));
        if(close(fd) == -1){
            perror("close fd");
            return 1;
        }
    }
    // stderr to file
    if(s->err){
        // Open in write-only mode. If file does not exist create it.
        // If file already exists truncate it to zero.
        // If new file is created, set permissions to
        // allow read, write, exec by user.
        if((fd = open(s->err, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU)) == -1){
            perror("open s->err");
            return 1;
        }
        fcntl(fd, F_SETFD, 1);
        dup2(fd, fileno(stderr));
        if(close(fd) == -1){
            perror("close fd");
            return 1;
        }
    }


	/**
	 * DONE: Finally, execute the command using the tokens 
	 * (see execute_command function provided)
	 */
    int result = execute_command(s->tokens);

	/**
	 * DONE: Close all filedescriptors needed and return status generated 
	 * by the command execution.
	 */

	// file descriptors already closed in if statements above

    return result;
}


/**
 * Executes a simple command (no pipes).
 */
// Returns the exit code of the command.
int execute_simple_command(simple_command *cmd) {
	pid_t pid;
	int status;
	int result = 0;
    
    //fprintf(stderr, "Excecute simple command: %s\n", cmd->tokens[0]);

	/**
	 * DONE: 
	 * Check if the command is builtin.
     */
    if (cmd->builtin){
    /* DONE:
	 * 1. If it is, then handle BUILTIN_CD (see execute_cd function provided) 
	 *    and BUILTIN_EXIT (simply return an appropriate exit status).
     */
        if(cmd->builtin == BUILTIN_CD)
            return execute_cd(cmd->tokens);
        else if(cmd->builtin == BUILTIN_EXIT)
            return -1;  // will cause exit from main loop
        else    // not a supported builtin
            return 1;
    }
    else{
    /* DONE:
	 * 2. If it isn't, then you must execute the non-builtin command. 
	 * - Fork a process to execute the nonbuiltin command 
	 *   (see execute_nonbuiltin function provided).
	 * - The parent should wait for the child.
	 *   (see wait man pages).
	 */
        pid = fork();
        if(pid == -1){
            perror("execute_simple_command: fork");
            return 1;
        }
        // In the child, execute the non-builtin command
        else if(pid == 0){
            exit(execute_nonbuiltin(cmd));
        }
        // In the parent, wait for the child and return the result
        else{
            if(wait(&status) == -1){
                perror("execute_simple_command: wait");
            }
            result = WEXITSTATUS(status);
            return result;
        }
	}
}


/**
 * Executes a complex command - it can have pipes (and optionally, other operators).
 */
// Returns 0 for normal execution, -1 for exit from main loop, and 1 for error.
int execute_complex_command(command *c) {
	
	/**
	 * DONE:
	 * Check if this is a simple command, using the scmd field.
	 * Remember that this will be called recursively, so when you encounter
	 * a simple command you should act accordingly.
	 * Execute nonbuiltin commands only. If it's exit or cd, you should not 
	 * execute these in a piped context, so simply ignore builtin commands. 
	 */

	if(c->scmd){
        return execute_simple_command(c->scmd);
    }


	/** 
	 * Optional: if you wish to handle more than just the 
	 * pipe operator '|' (the '&&', ';' etc. operators), then 
	 * you can add more options here. 
	 */

	if (!strcmp(c->oper, "|")) {
		
		/**
		 * DONE: Create a pipe "pfd" that generates a pair of file descriptors, 
		 * to be used for communication between the parent and the child.
		 * Make sure to check any errors in creating the pipe.
		 */
        int pfd[2];
        if(pipe(pfd) == -1){
            perror("execute_complex_command: pipe");
            return 1;
        }

			
		/**
		 * DONE: Fork a new process.
         */
        pid_t left_child_pid, right_child_pid;
        left_child_pid = fork();
        if(left_child_pid == -1){
            perror("execute_complex_command: fork left child");
            return 1;
        }
        /* DONE:
		 * In the child:
		 *  - close one end of the pipe pfd and close the stdout file descriptor.
		 *  - connect the stdout to the other end of the pipe (the one you didn't close).
		 *  - execute complex command cmd1 recursively. 
		 */
        // This child is the left side of the |
        // It reads from stdin as usual, but writes to the pipe
        if(left_child_pid == 0){
            // Close the read end of the pipe
            if(close(pfd[0]) == -1){
                perror("execute_complex_command: close(pfd[0]) in left child");
                exit(1);
            }
            // Connect stdout to the write end of the pipe
            if(dup2(pfd[1], fileno(stdout)) == -1){
                perror("execute_complex_command: \
                    dup2(pfd[1], fileno(stdout)) in left child");
                exit(1);
            }
            // pfd is now redundant, so close it
            if(close(pfd[1]) == -1){
                perror("execute_complex_command: close(pfd[1]) in left child");
                exit(1);
            }
            // execute complex command cmd1 recursively
            exit(execute_complex_command(c->cmd1));
        }
        /* Done:
         * In the parent: 
		 *  - fork a new process to execute cmd2 recursively.
         */
        right_child_pid = fork();
        if(right_child_pid == -1){
            perror("execute_complex_command: fork right child");
            return 1;
        }
        /*  DONE:
		 *  - In child 2:
		 *     - close one end of the pipe pfd (the other one than the first child), 
		 *       and close the standard input file descriptor.
		 *     - connect the stdin to the other end of the pipe (the one you didn't close).
		 *     - execute complex command cmd2 recursively. 
		 */
        // This child is the right side of the |
        // It writes to stdout as usual, but reads from the pipe
        if(right_child_pid == 0){
            // Close the write end of the pipe
            if(close(pfd[1]) == -1){
                perror("execute_complex_command: close(pfd[1]) in right child");
                exit(1);
            }
            // Connect stdin to the read end of the pipe
            if(dup2(pfd[0], fileno(stdin)) == -1){
                perror("execute_complex_command: \
                    dup2(pfd[0], fileno(stdin)) in right child");
                exit(1);
            }
            // pfd is now redundant, so close it
            if(close(pfd[0]) == -1){
                perror("execute_complex_command: close(pfd[0]) in right child");
                exit(1);
            }
            // execute complex command cmd2 recursively
            exit(execute_complex_command(c->cmd2));
        }
        /*  DONE:
         *  - In the parent:
		 *     - close both ends of the pipe. 
		 *     - wait for both children to finish.
		 */
		if(close(pfd[0]) == -1){
            perror("close(pfd[0]) in parent");
            return 1;
        }
        if(close(pfd[1]) == -1){
            perror("close(pfd[1]) in parent");
            return 1;
        }
        int status1, status2;
        pid_t pid1, pid2;
        pid1 = wait(&status1);
        pid2 = wait(&status2);
        
        if(pid1 == -1 || pid2 == -1){
            perror("wait");
            return 1;
        }

        //fprintf(stderr, "%d | %d\n", WEXITSTATUS(status1), WEXITSTATUS(status2));

        // If either child returned a -1 (255 in 2's complement),
        // then we should return -1 (exit from main loop)
        if(WEXITSTATUS(status1) == 255 || WEXITSTATUS(status2) == 255)
            return -1;
	}
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Read a user id and password from standard input, 
   - create a new process to run the validate program
   - use exec (probably execlp) to load the validate program.
   - send 'validate' the user id and password on a pipe, 
   - print a message 
        "Password verified" if the user id and password matched, 
        "Invalid password", or 
        "No such user"
     depending on the return value of 'validate'.

Setting the character arrays to have a capacity of 256 when we are only
expecting to get 10 bytes in each is a cheesy way of preventing most
overflow problems.
*/

#define MAXLINE 256
#define MAXPASSWD 10

// Strip out any part of the string after a \n
void strip(char *str, int capacity) {
    char *ptr;
    // find first occurence of \n
    if((ptr = strchr(str, '\n')) == NULL) {
        // \n not found, set last char to \0
        str[capacity - 1] = '\0';
    } else {
        // \n found, set it to \0
        *ptr = '\0';
    }
}


int main(void) {
    char userid[MAXLINE];
    char password[MAXLINE];

    /* Read a user id and password from stdin */
    printf("User id:\n");
    if((fgets(userid, MAXLINE, stdin)) == NULL) {
        fprintf(stderr, "Could not read from stdin\n"); 
        exit(1);
    }
    strip(userid, MAXPASSWD);

    printf("Password:\n");
    if((fgets(password, MAXLINE, stdin)) == NULL) {
        fprintf(stderr, "Could not read from stdin\n"); 
        exit(1);
    }
    strip(password, MAXPASSWD);

    /*Your code here*/

    // Create pipe
    // TODO: error checking for pipe
    int pipe_fd[2];
    pipe(pipe_fd);

    // Create process to run validate program
    // TODO: error checking for fork
    if(fork() == 0){
        // *****Child*****
        // Set stdin to be the read end of the pipe
        dup2(pipe_fd[0], fileno(stdin));
        // close the pipe fd's (not needed anymore)
        close(pipe_fd[0]);
        close(pipe_fd[1]);

        // start running validate
        execlp("validate", "validate", NULL);
        // TODO: error checking for execlp
    }

    // *****Parent*****
    // close the read end of the pipe
    close(pipe_fd[0]);

    // Send userid and password to validate
    // TODO: error checking for write
    write(pipe_fd[1], userid, 10);
    write(pipe_fd[1], password, 10);
    // done with writing
    close(pipe_fd[1]);

    // Wait for validate to finish
    int status = 1;
    // TODO: error checking for wait
    wait(&status);
    if(WIFEXITED(status)){
        status = WEXITSTATUS(status);
        // Print result of validate
        if(status == 0)
            printf("Password verified\n");
        else if(status == 1)
            printf("Validation error\n");
        else if(status == 2)
            printf("Invalid password\n");
        else if(status == 3)
            printf("No such user\n");
    }

    return 0;
}

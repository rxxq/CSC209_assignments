#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int numkids = 0;

/* Step 1: Write a signal handling function */
void handle_sigchld(int code){
    int chld_status;
    pid_t chld_pid;
    int chld_exitcode;

    if((chld_pid = wait(&chld_status)) == -1){
        perror("wait");
        exit(1);
    }
    if(WIFEXITED(chld_status)){
        chld_exitcode = WEXITSTATUS(chld_status);
        printf("child pid %d exited with status %d\n",
                chld_pid, chld_exitcode);
        numkids--;
    }

    return;
}



/* don't change the child code */
void do_child(int cnum) {
    printf("Child %d\n", cnum);
    sleep(2*cnum);
    // modified line below
    //sleep((time_t)(cnum*cnum/20));
    exit(cnum);
}

int main(int argc, char **argv) {
    int i;
    pid_t pid;
    struct sigaction act;

    if(argc != 2) {
        fprintf(stderr, "Usage: sigchld <num children>\n");
        exit(1);
    }
    numkids = atoi(argv[1]);

  /* Turn off buffering */
    setbuf(stdout, NULL);

  /* Step 2: Install signal handler */
    act.sa_handler = handle_sigchld;
    if(sigemptyset(&(act.sa_mask)) == -1){
        perror("sigemptyset"); return 1;
    }
    if(sigaddset(&(act.sa_mask), SIGCHLD) == -1){
        perror("sigaddset"); return 1;
    }
    act.sa_flags = 0;
    
    if(sigaction(SIGCHLD, &act, NULL) == -1){
        perror("sigaction"); return 1;
    }

  /* create some children */
  /* Don't change any code below this line */
  /* NOTE: This code was fixed to use k rather than numkids
   * as the stopping condition because numkids could change.*/
    int k = numkids;
    for (i = 0; i < k; i++) {
        if ((pid = fork()) == -1) {
            perror("fork"); exit(1);
            
        } else if (pid == 0) {
        /* child */
            printf("child %d created\n", i);
            do_child(i);
        }
    }

  /* compute for a while */
    i = 0;
    while(numkids > 0) {
        i++;
        if (i % 10000000 == 0)
            putchar('.');
    }
    printf("\ndone computing\n");

    return 0;
}


#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>


int main(){
    pid_t pid = getpid();
    char str[10000];

    gets(str);
    printf("%d printing to stdout: %s\n", pid, str);
    fprintf(stderr, "%d printing to stderr: %s\n", pid, str);

    return 0;
}

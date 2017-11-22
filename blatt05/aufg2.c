#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(void) {
    //a)
    pid_t pid = fork();
    if(pid == 0) { //child
        printf("My ID: %d\n", getpid());
        sleep(1);
        printf("My parent's ID %d\n", getppid());
        printf("Child is finished\n");
    }
    else {
        int status;
        waitpid(pid, &status, 0);
        printf("Parent finished.");
    }
}
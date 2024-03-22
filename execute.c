#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>

void execute(char command[256]) {
    int fd[2];
    int val = 0;
    pipe(fd);
    
    pid_t pid = fork();

    if (pid > 0) {
        // Parent (Shell)
        int *childStatus = 0;
        pid_t parentStatus = wait(childStatus);

    }
    else if (pid == 0) {
        // Child
        char *args[]={command, NULL};
        execvp(args[0],args);

        // TODO: Pass return value of execvp to parent process and return it to shell.c from there
        // TODO: Then call execute from shell.c
    }
}

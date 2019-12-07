#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

void run_command(char **tokens){
    int pid = fork();
    if(pid == 0){
        // Child fork
        printf("Fork running\n");
        execvp(tokens[0], tokens);
    }else{
        // Parent fork
        int status;
        wait(&status);
        printf("Status code: %d\n", status);
        printf("errno: %s\n", strerror(errno));
    }
}
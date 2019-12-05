#include <unistd.h>
#include <stdio.h>
#include<sys/wait.h>

void run_command(char **tokens){
    int pid = fork();
    if(pid == 0){
        // Child fork
        execvp(tokens[0], tokens);
    }else{
        // Parent fork
        int status;
        wait(&status);
    }
}
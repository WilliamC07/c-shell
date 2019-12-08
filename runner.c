#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "parse_input.h"

void run_command(char **tokens){
    if(strcmp(tokens[0], "cd") == 0){
        chdir(tokens[1]);
        if(errno){
            printf("%s\n", strerror(errno));
        }
    }else if(strcmp(tokens[0], "exit") == 0){
        exit(0);
    }else{
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
}

void run_commands(char **commands){
    int command_index = 0;
    while(commands[command_index] != NULL){
        char *command = commands[command_index];
        char **tokens = tokenize_command(command);
        run_command(tokens);
        free(tokens);
        command_index++;
    }
}
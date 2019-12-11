#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include "parse_input.h"

void run_command(char **tokens){
    if(strcmp(tokens[0], "cd") == 0){
        chdir(tokens[1]);
        if(errno){
            printf("%s\n", strerror(errno));
        }
    }else{
        int pid = fork();

        if(pid == 0){
            // Child fork

            // Handle redirection
            char *output = get_standard_output(tokens);
            u_int fd_output = -1;
            char *input = get_standard_input(tokens);
            u_int fd_input = -1;

            if(output != NULL){
                errno = 0;
                fd_output = open(output, O_CREAT | O_WRONLY, 0644);
                if(errno){
                    printf("\e[31m%s\e[0m\n", strerror(errno));
                }
                free(output);
            }

            if(input != NULL){
                errno = 0;
                fd_input = open(input, O_RDONLY);
                if(errno){
                    printf("\e[31m%s\e[0m\n", strerror(errno));
                }
                free(input);
            }

            if(fd_input != -1) dup2(fd_input, STDIN_FILENO);
            if(fd_output != -1) dup2(fd_output, STDOUT_FILENO);

            if(execvp(tokens[0], tokens) == -1){
                printf("\e[31mError running %s: %s\e[0m\n", tokens[0], strerror(errno));
                if(fd_input) close(fd_input);
                if(fd_output) close(fd_output);
                exit(1);
            }else{
                if(fd_input) close(fd_input);
                if(fd_output) close(fd_output);
            }
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

        // Special case for exit because we have to clean up memory before quitting the program
        if(strcmp(tokens[0], "exit") == 0){
            int token_index = 0;
            while(tokens[token_index] != NULL){
                free(tokens[token_index++]);
            }
            free(tokens);
            free(commands);
            exit(0);
        }

        // Run all other commands except "exit"
        if(redirection_parameters_given(tokens)){
            run_command(tokens);
        }

        // Clean up token memory for every command
        int token_index = 0;
        while(tokens[token_index] != NULL){
            free(tokens[token_index++]);
        }
        free(tokens);
        command_index++;
    }
    free(commands);
}
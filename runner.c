#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include "parse_input.h"
#define PIPE_READ 0
#define PIPE_WRITE 1

void free_items(char **items){
    int i = 0;
    while(items[i] != NULL){
        free(items[i]);
        i++;
    }
    free(items);
}

void run_command(char **tokens){
    char **tokens_to_run = tokens;
    int read_from_pipe_fd = -1;

    while(true) {
        // redirects and pipe
        char *output_file = get_standard_output(tokens_to_run);
        int fd_output = -1;
        char *input_file = get_standard_input(tokens_to_run);
        int fd_input = -1;

        int pipe_index = find_pipe(tokens_to_run);
        int pipe_file_descriptors[2] = {-1, -1};
        if(pipe_index != -1){
            // Pipe
            if(pipe(pipe_file_descriptors) == -1){
                printf("\e[31mFailed to pipe: %s\e[0m\n", strerror(errno));

            }
        }

        // Open redirect files
        if(output_file != NULL){
            errno = 0;
            fd_output = open(output_file, O_CREAT | O_WRONLY, 0644);
            if(errno){
                printf("\e[31m%s\e[0m\n", strerror(errno));
            }
            free(output_file);
        }
        if(input_file != NULL){
            errno = 0;
            fd_input = open(input_file, O_RDONLY);
            if(errno){
                printf("\e[31m%s\e[0m\n", strerror(errno));
            }
            free(input_file);
        }

        // We do not support both output to file and pipe
        if(output_file != NULL && pipe_file_descriptors[PIPE_WRITE] != -1){
            printf("\e[31mCannot pipe and redirect to file. Command quit!\e[0m\n");
            return;
        }

        int pid = fork();
        if(pid == 0){
            // Child fork

            // Get source from pipe
            if (read_from_pipe_fd != -1) dup2(read_from_pipe_fd, STDIN_FILENO);
            if (pipe_file_descriptors[PIPE_WRITE] != 0) dup2(pipe_file_descriptors[PIPE_WRITE], STDOUT_FILENO);
            // Redirect input
            if (fd_input != -1) dup2(fd_input, STDIN_FILENO);
            // Redirect output
            if (fd_output != -1) dup2(fd_output, STDOUT_FILENO);

            // Run
            if(execvp(tokens_to_run[0], tokens_to_run) == -1){
                printf("\e[31mError running %s: %s\e[0m\n", tokens_to_run[0], strerror(errno));
                if(fd_input != -1) close(fd_input);
                if(fd_output != -1) close(fd_output);
                exit(1);
            }
        }else{
            // parent
            // close pipe
            if(pipe_index != -1) close(pipe_file_descriptors[PIPE_WRITE]);
            if(read_from_pipe_fd != -1){
                close(read_from_pipe_fd);
            }

            int status;
            wait(&status);

            int i = 0;
            while(tokens_to_run[i] != NULL){
                free(tokens_to_run[i++]);
            }

            // If there isn't a pipe symbol, that means no other command can run
            if(pipe_index == -1){
                break;
            }
        }
        // pipe was set, the next command needs to run using the pipe READ
        read_from_pipe_fd = pipe_file_descriptors[PIPE_READ];
        // Add 1 since the next command token is after the pipe
        tokens_to_run = tokens + pipe_index + 1;
    }
}

void run_commands(char **commands){
    int command_index = 0;
    while(commands[command_index] != NULL){
        char *command = commands[command_index];
        char **tokens = tokenize_command(command);

        // Special case for exit because we have to clean up memory before quitting the program
        if(strcmp(tokens[0], "exit") == 0){
            free_items(tokens);
            free_items(commands);
            exit(0);
        }else if(strcmp(tokens[0], "cd") == 0){
            errno = 0;
            chdir(tokens[1]);
            if(errno){
                printf("%s\n", strerror(errno));
            }
            free_items(tokens);
        }else{
            // Run all other commands except "exit"
            if(redirection_parameters_given(tokens)){
                run_command(tokens);
                free(tokens);
            }
        }
        command_index++;
    }
    free_items(commands);
}
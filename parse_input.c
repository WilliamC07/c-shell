#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * Removes the end of line character ('\n') from the user input. This should be run before working.
 *
 * TODO: Make it divide into multiple strings if user gave semicolon (';') in the input
 * @param input Input the user gave
 */
void sterialize_input(char *input) {
    input[(strchr(input, '\n') - input)] = '\0';
}

int count_occurance(char *string, char character){
    if(string == NULL || string[0] == '\0'){
        return 0;
    }

    int counter = 0;
    for(int index = 0; string[index] != NULL; index++){
        if(string[index] == character){
            counter++;
        }
    }

    return counter;
}

int num_args(char *command){
    if(command == NULL || command[0] == '\0'){
        return 0;
    }

    // Add one since spaces comes after end of argument
    return count_occurance(command, ' ') + 1;
}

/**
 * This is NOT a reimplementation of string.h strsep.
 * This divides up the given string and separates it by replacing each occurrence of the delimiter that is not surrounded by quotes with a '\0'.
 * This assumes the quotes are balanced. Each separated part is called a block.
 *
 * @param input Will be modified
 * @param delimiter
 * @return Array of pointer to each block.
 */
char **split_handle_quotes(char *string, char delimiter){
    int index = 0;
    // This will result in extra space if there is a semicolon in quotes
    // Add 2: to make last pointer NULL to show there are no more commands and
    //        at least two space if the user enters one command (no semicolon)
    char **commands = calloc(count_occurance(string, delimiter) + 2, sizeof(char *));
    int command_start_index = 0;  // The index of input in which the current command starts at
    int command_counter = 0;  // Keep track of how many commands were given so far
    bool in_quotes = false;

    for(;; index++){
        if(string[index] == '"' && index != 0 && string[index - 1] != '\\') {
            // User entered a quote (handles escaped quote \"
            in_quotes = !in_quotes;
        }else if(!in_quotes && (string[index] == delimiter || string[index] == '\0')){
            // End of command
            commands[command_counter++] = string + command_start_index;
            // Move the start to the next command
            command_start_index = index + 1;  // Skip over the semicolon

            if(string[index] == '\0'){
                // End of input (also the end of the last command)
                return commands;
            }else{
                string[index] = '\0';
            }
        }
    }
}

/**
 * Parses a command into separate parts divided into name of program and arguments passed in.
 * Delimiter of " "
 * Assuming that the length of the command is <1000 characters and <20 command line argument.
 *
 * @param input
 * @return
 */
char **tokenize(char *command) {
    char *current = command;
    char *token;
    int tokens_read = 0;
    // Use calloc so it automatically includes the end of string character.
    // Add one since we need a NULL pointer to signify there aren't any more parameters in execvp
    char **tokens = calloc(sizeof(char *), num_args(command) + 1);

    while (1) {
        token = strsep(&current, " ");
        if (token == NULL) {
            return tokens;
        }
        // Use calloc so it automatically includes the end of string character.
        *(tokens + tokens_read) = calloc(1000, sizeof(char));
        strcpy(tokens[tokens_read], token);
        tokens_read++;
    }
}
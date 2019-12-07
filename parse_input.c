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
 * Will separate the input into an array of commands (separated by ';').
 * This is a worse implementation of string.h strsep that handles quotes.
 * For example, the command:
 *  echo -e ";"; echo -e "\x1BC34 Apples"
 *
 * Will separate into the two commands:
 *  1. echo -e ";"
 *  2. echo -e "\x1BC34 Apples"
 *
 * @param input
 * @return
 */
char **get_commands(char *input){
    int index = 0;
    // This will result in extra space if there is a semicolon in quotes
    // Add 2: to make last pointer NULL to show there are no more commands and
    //        at least two space if the user enters one command (no semicolon)
    char **commands = calloc(count_occurance(input, ';') + 2, sizeof(char *));
    int command_start_index = 0;  // The index of input in which the current command starts at
    int command_counter = 0;  // Keep track of how many commands were given so far
    bool in_quotes = false;

    for(;; index++){
        if(input[index] == '"' && index != 0 && input[index - 1] != '\\') {
            // User entered a quote (handles escaped quote \"
            in_quotes = !in_quotes;
        }else if(!in_quotes && input[index] == ';'){
            // End of command
            commands[command_counter++] = input + command_start_index;
            // Move the start to the next command
            command_start_index = index + 1;  // Skip over the semicolon
            input[index] = '\0';  // Signal it is the end of the command
        }else if(!in_quotes && input[index] == '\0'){
            // End of input, which also means end of the last command
            // End of command
            commands[command_counter] = input + command_start_index;
            break;
        }
    }

    return commands;
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
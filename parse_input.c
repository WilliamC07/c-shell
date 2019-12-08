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
    for(int index = 0; string[index] != '\0'; index++){
        if(string[index] == character){
            counter++;
        }
    }

    return counter;
}

/**
 * Inserts the given character into the string at the given index. This pushes all subsequent characters over.
 * The destination space should have all spaces after the '\0' filled with '\0' and have enough space to insert into.
 */
void insert_character(char *destination, char insert_char, int position){
    // To shift over, we will move backwards
    int index = strlen(destination);
    for(; index > position; index--){
        destination[index] = destination[index - 1];
    }

    destination[position] = insert_char;
}

/**
 * Will separate the input into an array of commands (separated by ';').
 * This is NOT a reimplementation of string.h strsep.
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
    // This will result in extra space if there is a semicolon in quotes
    // Add 2: to make last pointer NULL to show there are no more commands and
    //        at least two space if the user enters one command (no semicolon)
    char **commands = calloc(count_occurance(input, ';') + 2, sizeof(char *));
    int command_start_index;  // The index of input in which the current command starts at
    int command_counter = 0;  // Keep track of how many commands were given so far
    bool in_quotes = false;

    // Find the index of the start of the first command
    // In bash and zsh (not sure about sh), leading space means don't store in history. This shell doesn't implement this feature
    int index = 0;
    while(input[index] == ' ' || input[index] == ';'){
        index++;
    }
    command_start_index = index;

    while(true){
        if(input[index] == '"') {
            if(index != 0 && input[index - 1] != '\\'){
                // The user entered a quote but not an escaped quote (\")
                in_quotes = !in_quotes;
            }
            index++;
        }else if(!in_quotes && (input[index] == ';' || input[index] == '\0')){
            // The user finished entering a command

            // Store the start of the current command
            commands[command_counter++] = input + command_start_index;

            if(input[index] == '\0'){
                // End of input (also the end of the last command)
                return commands;
            }else{
                // End the command
                input[index] = '\0';
                // make sure the command does not have trailing spaces
                int end_index = index;
                while(input[--end_index] == ' '){
                    input[end_index] = '\0';
                }

                // Move the index to the next command
                index++;
                // Do not have the next command lead with semicolon or spaces
                // Prevents us from thinking a space or a semicolon is a command
                while(input[index] == ';' || input[index] == ' '){
                    index++;
                }
                command_start_index = index;
            }
        }else{
            index++;
        }
    }
}

/**
 * Commands do not include '<' '>' or '|'.
 * @param command
 * @return
 */
char ** tokenize_command(char *command){
    // Spaces separate command arguments. This may give more space than needed if there are spaces surrounded by quotes.
    // Add 2 since last element in array must be NULL to denote end of arguments and
    // there is at least 1 argument for every command
    char ** tokens = calloc(count_occurance(command, ' ') + 2, sizeof(char *));
    // The index of command in which the current argument starts at
    // The command is guaranteed to not start with a space due to the get_commands(char *) implementation
    int token_start_index = 0;
    int token_counter = 0;  // Keep track of how many arguments were given so far
    bool in_quotes = false;
    int index = 0;

    while(true){
        if(command[index] == '"') {
            if(index != 0 && command[index - 1] != '\\'){
                // The user entered a quote but not an escaped quote (\")
                in_quotes = !in_quotes;
            }
            index++;
        }else if(!in_quotes && (command[index] == ' ' || command[index] == '\0')){
            // The user finished entering an argument

            // Store the start of the current token
            tokens[token_counter++] = command + token_start_index;

            if(command[index] == '\0'){
                tokens[token_counter] = NULL;
                return tokens;
            }else{
                // end the token
                command[index] = '\0';

                // Move the index to the next token (moving past the space)
                index++;

                // Do not have the next token lead with spaces
                while(command[index] == ' '){
                    index++;
                }
                token_start_index = index;
            }
        }else{
            index++;
        }
    }
}
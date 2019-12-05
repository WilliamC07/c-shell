#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Removes the end of line character ('\n') from the user input. This should be run before working.
 *
 * TODO: Make it divide into multiple strings if user gave semicolon (';') in the input
 * @param input Input the user gave
 */
void sterialize_input(char *input) {
    input[(strchr(input, '\n') - input)] = '\0';
}

int num_args(char *command){
    if(command == NULL || command[0] == '\0'){
        return 0;
    }

    // Start at 1 since spaces come after each argument
    int counter = 1;
    for(int index = 0; index < sizeof(command); index++){
        if(command[index] == ' '){
            counter++;
        }
    }
    return counter;
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
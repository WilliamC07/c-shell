#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ntsid.h>

/**
 * Removes the end of line character ('\n') from the user input. This should be run before working.
 *
 * TODO: Make it divide into multiple strings if user gave semicolon (';') in the input
 * @param input Input the user gave
 */
void sterialize_input(char *input) {
    input[(strchr(input, '\n') - input)] = '\0';

    // make sure quotes are balanced
    u_int length = strlen(input);
    u_int index = 0;
    int counter = 0;
    for(; index < length; index++){
        if(input[index] == '"'){
            // only not considered a quote when it is escaped
            // \\" is not escaped, but \" is
            if(!(input[index - 1] == '\\' && input[index - 2] != '\\')){
                counter++;
            }
        }
    }
    if(counter % 2 == 1){
        // Not balanced
        printf("\e[31mBalance your quotes. Command failed to be called, please fix.\e[0m\n");
        input[0] = '\0';
    }
}

unsigned int count_occurance(char *string, char character){
    if(string == NULL || string[0] == '\0'){
        return 0;
    }

    unsigned int counter = 0;
    for(int index = 0; string[index] != '\0'; index++){
        if(string[index] == character){
            counter++;
        }
    }

    return counter;
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
    u_int command_index = 0;

    u_int index = 0;
    while(input[index] != '\0'){
        if(input[index] == ' ' || input[index] == ';'){
            // Ignore white spaces and lone semicolon
            index++;
        }else if(input[index] == '\0') {
            return commands;
        }else{
            // all characters other than spaces are considered the start of a command
            u_int command_start_index = index;

            // Semicolon not in quotes signals the end of the command
            while(input[index] != ';' && input[index] != '\0'){
                // If we find a quote character, skip to end of quotes
                if(input[index] == '"') {
                    // Move past quote character so we can find last quote character that wasn't escaped
                    index++;
                    while (!(input[index] == '"' && input[index - 1] != '\\')) {
                        index++;
                    }
                }
                index++;
            }

            // Minus one to not include semicolon or end of string character in command
            int end_index = index;
            // Remove trailing white spaces
            while(end_index != 0 && (input[end_index] == ' ' || input[end_index] == '\0' || input[end_index] == ';')){
                end_index--;
            }
            // Add two: one for end of string character and one for length of [a, b] = b - a + 1
            u_int length_command = end_index - command_start_index + 1;
            if(length_command != 2){
                // Not all white spaces, so command was given
                char *command = calloc(length_command, sizeof(char));
                strncpy(command, input + command_start_index, length_command);
                commands[command_index++] = command;
            }

            if(input[index] == ';'){
                index++;
            }else if(input[index] == '\0'){
                return commands;
            }
        }
    }

    return commands;
}

/**
 * Returns allocated memory of quote at the given index.
 *
 * @param command
 * @param start_index Pointer to the start of the quote character
 * @return
 */
char * parse_quote_token(char *quote_start, u_int *char_parsed){
    // Find length of the quote
    // We will consider backslash followed by character as two character when it really should just be 1 character since
    // we don't handle every escape character case
    u_int length = 1;
    while(quote_start[length] != '"' || quote_start[length - 1] == '\\'){
        length++;
    }

    // Need to tell the parser how many char we are reading off
    *char_parsed = length;

    // Do not minus 1 from offset because we want to replace the ending quote with a end of string character
    char *token = calloc(length, sizeof(char));
    u_int index_token = 0;
    u_int offset = 1;
    while(offset != length){
        if(quote_start[offset] == '\\' && quote_start[offset - 1] != '\\'){
            switch(quote_start[offset + 1]){
                case '\t':
                    token[index_token++] = '\t';
                    break;
                case 'n':
                    token[index_token++] = '\n';
                    break;
                case '"':
                    token[index_token++] = '\"';
                    break;
                default:
                    // Add 2 to the token index since we are adding both backslash and the following character
                    token[index_token++] = quote_start[offset];
                    token[index_token++] = quote_start[offset + 1];
                    break;
            }
            // Skip the current backslash and next escaped character
            offset += 2;
        }else{
            token[index_token++] = quote_start[offset];
            offset++;
        }
    }
    return token;
}

/**
 * Returns allocated memory of argument at the given index.
 *
 * Things that ends a token (command line argument):
 * - White space
 * - End of string character
 * - Redirecting (>, <)
 * - Piping (|)
 * @param token_start
 * @param char_parsed
 * @return
 */
char * parse_regular_token(char *token_start, u_int *char_parsed){
    u_int length = 1;
    while(token_start[length] != ' ' && token_start[length] != '\0' &&
        token_start[length] != '>' && token_start[length] != '<' && token_start[length] != '|'){
        length++;
    }

    *char_parsed = length;

    // Add 1 for end of character string
    char *token = calloc(length + 1, sizeof(char));
    strncpy(token, token_start, length);
    return token;
}

/**
 * Commands do not include '<' '>' or '|'.
 * @param command
 * @return
 */
char ** tokenize_command(char *command){
    // This may result in more tokens than there actually are because the characters we look for may be in quotes
    // Add 2 since last element in array must be NULL to denote end of arguments and there must be at least 1 argument
    // The following characters: ">", "<", and "|" are special operations and must be their own token
    u_int amount_tokens = count_occurance(command, ' ') + count_occurance(command, '<') + count_occurance(command, '>') + count_occurance(command, '|') + 2;
    char **tokens = calloc(amount_tokens + 1, sizeof(char *));
    u_int token_index = 0;
    u_int command_index = 0;
    while(command[command_index] != '\0'){
        switch(command[command_index]){
            case '"': {
                // Double quotes represents one token
                // It is assumed to be balanced
                // User has entered a quote (but not an escaped quote), signaling the start of the quote token
                u_int char_parsed;
                char *token = parse_quote_token(command + command_index, &char_parsed);
                tokens[token_index++] = token;
                // We parsed the entire token, so move index over
                // Add 1 since we want to move past the ending double quote
                command_index += char_parsed + 1;
                break;
            }
            case ' ': {
                // ignore white spaces
                command_index++;
                break;
            }
            case '>':
            case '<':
            case '|': {
                // These are special operational character, so they need to be its own token
                // Since we will check the 0th index of every token for the above special characters, we don't need '\0'
                char *token = malloc(1);
                token[0] = command[command_index];
                tokens[token_index++] = token;
                command_index++;
                break;
            }
            default: {
                // Regular strings goes up to the white space signaling end of the token
                u_int char_parsed;
                char *token = parse_regular_token(command + command_index, &char_parsed);
                tokens[token_index++] = token;
                // Add 1 since we move past the last read character
                command_index += char_parsed;
            }
        }
    }
    return tokens;
}

bool redirection_parameters_given(char ** tokens){
    int index = 0;
    while(tokens[index] != NULL){
        if(tokens[index][0] == '>' || tokens[index][0] == '<' || tokens[index][0] == '|'){
            if(index == 0 || tokens[index + 1] == NULL){
                // Missing parameter either right or left
                // Print red and reset color after
                printf("\e[31mMalformed direction or pipe. Command failed to be called, please fix.\e[0m\n");
                return false;
            }
        }
        index++;
    }
    return true;
}

u_int find_token_index(char **tokens, char *needle){
    u_int index = 0;
    while(tokens[index] != NULL){
        if(strcmp(tokens[index], needle) == 0){
            return index;
        }
        index++;
    }
    return -1;
}

char * get_standard_output(char **tokens){
    u_int output_token_index = find_token_index(tokens, ">");
    if(output_token_index == -1){
        // No ">"
        return NULL;
    }

    // Get given file name
    u_int file_name_length = strlen(tokens[output_token_index + 1]) + 1;  // add 1 for end of string character
    char *file_name = calloc(file_name_length, sizeof(char));
    strcpy(file_name, tokens[output_token_index + 1]);

    // Free the tokens that we read from
    free(tokens[output_token_index]);
    free(tokens[output_token_index + 1]);

    // Shift all future tokens over
    u_int index = output_token_index;
    while(tokens[index + 2] != NULL){
        tokens[index] = tokens[index + 2];
        index++;
    }
    // End the list of tokens
    tokens[index] = NULL;

    return file_name;
}

char * get_standard_input(char **tokens){
    u_int input_token_index = find_token_index(tokens, "<");
    if(input_token_index == -1){
        // No ">"
        return NULL;
    }

    // Get given file name
    u_int file_name_length = strlen(tokens[input_token_index + 1]) + 1;  // add 1 for end of string character
    char *file_name = calloc(file_name_length, sizeof(char));
    strcpy(file_name, tokens[input_token_index + 1]);

    // Free the tokens that we read from
    free(tokens[input_token_index]);
    free(tokens[input_token_index + 1]);

    // Shift all future tokens over
    u_int index = input_token_index;
    while(tokens[index + 2] != NULL){
        tokens[index] = tokens[index + 2];
        index++;
    }
    // End the list of tokens
    tokens[index] = NULL;

    return file_name;
}
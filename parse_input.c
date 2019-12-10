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
                commands[command_counter] = NULL;
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
        if(quote_start[offset] == '\\'){
            switch(quote_start[offset + 1]){
                case '\\':
                    token[index_token++] = '\\';
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

char * parse_regular_token(char *token_start, u_int *char_parsed){
    u_int length = 1;
    while(token_start[length] != ' ' && token_start[length] != '\0'){
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
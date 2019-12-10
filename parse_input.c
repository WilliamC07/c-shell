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
 * If the user enters something in quotes:
 * 1. The leading and trailing quotes need to be removed.
 * 2. Escaped quotes (\") needs to be converted to just (")
 * @param string
 */
char *parse_if_quotes(char *string){
    unsigned int original_size = strlen(string);

    // The only token we need to parse are those starting with quotes.
    // The shell will only deal with those.
    // TODO: Maybe replace "~" with home and add environment variables. Probably don't have time for this
    if('\0' == string[0] || !(string[0] == '"' && string[original_size - 1] == '"')){
        char *copied = calloc(original_size + 1, sizeof(char));  // Add 1 for end of line character
        strcpy(copied, string);
        return copied;
    }

    unsigned int number_escaped_quotes = 0;

    // count the number of escaped quotes
    // original_size - 1 because the last two characters cannot be an escaped quote (or else the string is malformed)
    for(int i = 0; i < original_size - 1; i++){
        if(string[i] == '"' && string[i - 1] == '\\'){
            number_escaped_quotes++;
        }
    }

    // Minus two for the start and end quote
    // Minus number_escaped_quotes since those will become just quotes (\" -> ")
    // Add one for end of line character
    // calloc so it automatically gives us the end of string character
    char *new_string = calloc(original_size - 2 - number_escaped_quotes + 1, sizeof(char));

    // Copy over content
    int new_index = 0;  // Index of new_string. Start at 1 to skip first double quote
    int old_index = 1;  // Index of the original_string
    // original_size - 1 to skip last double quote
    for(; old_index < original_size - 1; old_index++, new_index++){
        // "old_index + 1 != original_size - 1" Deals with "\\" --> \ and not "\\" --> \"
        if(string[old_index] == '\\' && string[old_index + 1] == '"' && old_index + 1 != original_size - 1){
            // Skip over
            old_index++;
            new_string[new_index] = '"';
        }else{
            new_string[new_index] = string[old_index];
        }
    }

    return new_string;
}

/**
 * If the user enters a ">" or "<" or "|" that is not in quotes, it has to be its own token
 * @param string
 * @return
 */
char *parse_if_operation(char *string){

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
char * parse_quote(char *quote_start){
    // Find length of the quote
    // We will consider backslash followed by character as two character when it really should just be 1 character since
    // we don't handle every escape character case
    u_int length = 1;
    while(quote_start[length] != '"' || quote_start[length - 1] == '\\'){
        length++;
    }

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
    char **tokens = calloc(amount_tokens, sizeof(char *));

    int command_index = 0;
    while(command[command_index] != '\0'){
        // Double quotes that are not escaped represents one token
        if(command[command_index] == '"' && (command_index != 0 ? command[command_index] != '\\' : false)){
            // User has entered a quote (but not an escaped quote), signaling the start of the quote token

        }

        // ignore white spaces
        if(command[command_index] == ' '){
            command_index++;
            continue;
        }



        command_index++;
    }

//    // Spaces separate command arguments. This may give more space than needed if there are spaces surrounded by quotes.
//    // there is at least 1 argument for every command
//    char ** tokens = calloc(amount_tokens, sizeof(char *));
//    // The index of command in which the current argument starts at
//    // The command is guaranteed to not start with a space due to the get_commands(char *) implementation
//    int token_start_index = 0;
//    int token_counter = 0;  // Keep track of how many arguments were given so far
//    bool in_quotes = false;
//    int index = 0;
//
//    while(true){
//        if(command[index] == '"') {
//            if(index != 0 && command[index - 1] != '\\'){
//                // The user entered a quote but not an escaped quote (\")
//                in_quotes = !in_quotes;
//            }
//            index++;
//        }else if(!in_quotes && (command[index] == ' ' || command[index] == '\0')){
//            // The user finished entering an argument
//
//            if(command[index] == '\0'){
//                tokens[token_counter++] = parse_if_quotes(command + token_start_index);
//                tokens[token_counter] = NULL;
//                return tokens;
//            }else{
//                // end the token
//                command[index] = '\0';
//
//                // Move the index to the next token (moving past the space)
//                index++;
//
//                // Do not have the next token lead with spaces
//                while(command[index] == ' '){
//                    index++;
//                }
//
//                // Store the start of the current token
//                tokens[token_counter++] = parse_if_quotes(command + token_start_index);
//                token_start_index = index;
//            }
//        }else{
//            index++;
//        }
//    }
}
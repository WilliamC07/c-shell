# (つ ツ )つ Shell
##### by William Cao

### Features
- Forks and executes commands
- Supports semicolons (run multiple commands in one input). You can spam semicolons with no commands and it will still work!
- Supports white spaces. You can spam white spaces everywhere and it will still work!
    - If you start a command with a white space, it still goes to history.
- Can redirect using "<", ">"
    - Will create files if missing.
    - Will warn user if attempting to redirect STDIN with missing file.
    - Redirect not supported for ```cd```
    - Command must be left and source file must be to right of command.
- Can chain pipes
    - Cannot pipe and redirect. Example: ```echo "as" > text.txt | wc```
    - Pipe must come after redirects. This will not work: ```echo | < a.txt wc``` (you have to do Control-d to end; but zsh behaves like this too).
- Supports double quotes
    - Escaping are supported only for:
        - "\t" --> tab character
        - "\n" --> new line character
        - "\"" --> Escapes double quote
    - These escape sequences must be in quotes or it will not work!
    - If a file/directory has a space in it, you can just wrap it in double quotes. Example: ```rm "file space.txt"``` and it will work!
- Supports ```cd``` and ```exit```
    - cd only accepts two argument( cd and next directory).
    - "~" is not supported
- Some very nice blue colored current directory text and red colored error messages. Wow!
- Makes sure you balanced all your quotes! (Gives you error message if not and command will not run).

### Bugs
- Entering EOF (Control-D) midline will break the program (segmentation fault).
- Escaping quotes outside of quotes will segfault. Example segfault: ```echo \""asd"```

### Files & Function Headers:
```
parse_input.c  
    Handles all line parsing functions  
    /*======== void sterialize_input() ==========  
    Inputs: char *input  
    Returns: No return  
    
    Removes the end of line character of the input  
    Makes sure all quotes are balanced. Will print an error message if not balanced and the entire input will not run.
    ====================*/
    
    /*======== unsigned int count_occurance() ==========  
    Inputs: char *string
            char character
    Returns: Number of times the given character appears. -1 if none found.
    
    Counts the amount of times the given character appears in the given string.
    ====================*/
    
    /*======== char **get_commands() ==========  
    Inputs: char *input
    Returns: Array of strings where each entry is a command to be run.
    
    Strips white spaces and semicolons off each command. 
    ====================*/
    
    /*======== char *parse_quote_token() ==========  
    Inputs: char *quote_start  
            int *char_parsed  
    Returns: String with escaped characters converted and starting and ending double quote removed. (Allocated to heap)  
      
    char_parsed is set to the amount of characters the function read through. So if quote_start is ```"apples\n"```, it will return 10.
    ====================*/
    
    /*======== char *parse_regular_token() ==========  
    Inputs: char *token_start
            int *char_parsed
    Returns: String of the given input. (Allocated to heap)  
    
    char_parsed is set to the amount of characters the function read through. So if toke_start is ```echo```, it will return 4.
    ====================*/
    
    /*======== char **tokenize_command() ==========  
    Inputs: char *command  
    Returns: Array of string where each entry is a token (a command argument)  
    
    Takes in a command and divides it up into tokens (command arguments). It will remove all trailing, leading spaces and semicolons.
    ====================*/
    
    /*======== char **tokenize_command() ==========  
    Inputs: char *command  
    Returns: Array of string where each entry is a token (a command argument)  
    
    Takes in a command and divides it up into tokens (command arguments). It will remove all trailing, leading spaces and semicolons.
    ====================*/
    
    /*======== bool redirection_parameters_given ==========  
    Inputs: char **tokens  
    Returns: true if all arguments for ">", "<", and "|" are given; false otherwise     
    
    Uses stdbool.h  
    A argument will be missing if the element to right or left is empty (index 0) or if it is NULL (end of command)
    ====================*/
    
    /*======== int find_token_index() ==========  
    Inputs: char **tokens  
            char *needle
    Returns: Returns index of the given token in the tokens. -1 if not found.
    ====================*/
    
    /*======== int find_token_index() ==========  
    Inputs: char **tokens  
            char *needle
    Returns: Returns index of the given token in the tokens. -1 if not found.
    ====================*/
    
    /*======== char * get_standard_output ==========  
    Inputs: char **tokens  
    Returns: Returns allocated string of output file name. NULL if no redirect given.
    ====================*/
    
    /*======== char * get_standard_input ==========  
    Inputs: char **tokens  
    Returns: Returns allocated string of input file name. NULL if no redirect given.
    ====================*/
    
    /*======== int find_pipe ==========  
    Inputs: char **tokens  
    Returns: Returns index of pipe. -1 if none found  
    
    Will free and replace the token with NULL.
    ====================*/
    
runner.c
    Handles execution of program.  
    /*======== void free_items() ==========  
    Inputs: char **items  
    Returns: void
    
    Free memory for every token in items (each entry) then frees the pointer to these tokens. 
    ====================*/
    
    /*======== void run_command() ==========  
    Inputs: char **tokens  
    Returns: void  
    
    Takes in the given command, open files and forks and execvp commands. Handles filetable logic and closing file descriptors as well. 
    ====================*/
    
    /*======== void run_commands() ==========  
    Inputs: char **commands 
    Returns: void  
    
    Takes in the given commands, and executes them by calling void run_command(char **tokens); exception to ```exit``` and ```cd```, since those cannot be run using execvp, so they are handled here.
    ====================*/

main.c
    /*======== int main() ==========  
    Inputs: void
    Returns: void  
    
    Runs the program
    ====================*/
```
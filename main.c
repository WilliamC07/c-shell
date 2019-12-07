#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "parse_input.h"
#include "runner.h"

void test(){
//    char ls_command[] = "echo 1; echo 2 ; echo 3;echo 4;echo 5;";
//    int arguments_num = num_args(ls_command);
//    printf("Tested %s\n Arguments: %d Expected %d\n", ls_command, arguments_num, 3);
//    printf("Running command %s\n", ls_command);
//    char ** arguments = tokenize(ls_command);
//    execvp(arguments[0], arguments);

    char input[] = "echo 1; echo 2 ; echo -e \";\" ;echo -e \"\\x1B[34mApples\"";
    char ** commands = get_commands(input);
    for(int i = 0; i < 4; i++){
        printf("Command: %s\n", commands[i]);
    }
}

int main() {
    test();
    return 0;

//    while (1) {
//        char input[500] = {'\0'};
//
//        printf("$ ");
//        fgets(input, sizeof(input), stdin);
//        sterialize_input(input);
//        printf("You entered: %s \n", input);
//
//        run_command(tokenize(input));
//        // Remove new line character of input
//
//    }
    return 0;
}
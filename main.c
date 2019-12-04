#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "parse_input.h"

void test(){
    char ls_command[] = "ls -l -a";
    int arguments_num = num_args(ls_command);
    printf("Tested %s\n Arguments: %d Expected %d\n", ls_command, arguments_num, 3);
    printf("Running command %s\n", ls_command);
    char ** arguments = tokenize(ls_command);
    execvp(arguments[0], arguments);
}

int main() {
    test();

//    while (1) {
//        char input[500] = {'\0'};
//
//        printf("$ ");
//        fgets(input, sizeof(input), stdin);
//
//        // Remove new line character of input
//
//        printf("You entered: %s \n", input);
//    }
    return 0;
}
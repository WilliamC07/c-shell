#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "parse_input.h"
#include "runner.h"
#include <stdlib.h>
#include <limits.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void test(){
//    char ls_command[] = "echo 1; echo 2 ; echo 3;echo 4;echo 5;";
//    int arguments_num = num_args(ls_command);
//    printf("Tested %s\n Arguments: %d Expected %d\n", ls_command, arguments_num, 3);
//    printf("Running command %s\n", ls_command);
//    char ** arguments = tokenize(ls_command);
//    execvp(arguments[0], arguments);

//    char input[] = "echo 1;echo 2 ; echo -e \"; echo Apple\" ;echo -e \"\\x1B[34m ;;Apples \"";
//    char input[] = "echo -e \"\\x1B[34m ;;Apples \"";
//    char ** commands = get_commands(input);
//    for(int i = 0; i < 4; i++){
//        printf("Command: .%s.\n", commands[i]);
//        char ** tokens = tokenize_command(commands[i]);
//        int t_i = 0;
//        while(tokens[t_i] != NULL){
//            printf("token: %s\n", tokens[t_i]);
//            t_i++;
//        }
//        free(tokens);
//    }
//    free(commands);
}

int main() {
//    test();
//    return 0;

    while (1) {
        char input[500] = {'\0'};
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        printf("\e[34m%s\n", cwd);
        printf("$\e[m ");

        char result = fgets(input, sizeof(input), stdin);
        if(result == NULL){
            // Given End Of File (Control D)
            printf("\n"); // Flush the dollar sign from previous printf
            exit(0);
        }
        sterialize_input(input);
        char ** commands = get_commands(input);
        run_commands(commands);
        free(commands);
    }
    return 0;
}
#pragma clang diagnostic pop
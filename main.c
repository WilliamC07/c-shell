#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "parse_input.h"
#include "runner.h"
#include <stdlib.h>
#include <limits.h>

int main() {
    while (1) {
        char input[500] = {'\0'};
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        printf("\e[34m%s $\e[m ", cwd);
        // force print the cwd and dollar sign or else redirecting to this program will spam all output at the very end
        fflush(stdout);

        char *result = fgets(input, sizeof(input), stdin);
        if(result == NULL){
            // Given End Of File (Control D)
            exit(0);
        }
        sterialize_input(input);
        char ** commands = get_commands(input);

        run_commands(commands);
    }
}
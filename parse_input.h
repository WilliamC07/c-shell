#include <sys/types.h>
#include <stdbool.h>

void sterialize_input(char *input);

char **get_commands(char *input);

char ** tokenize_command(char *command);

bool redirection_parameters_given(char ** tokens);

char * get_standard_output(char **tokens);

char * get_standard_input(char **tokens);

int find_pipe(char **tokens);
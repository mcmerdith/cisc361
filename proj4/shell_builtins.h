#pragma once

#define BUILTINCOUNT 13

#define PrintStatus(command) printf("Executing built-in command [%s]\n", command)
#define TooFewArgs(command) fprintf(stderr, "%s: Too few arguments.\n", command)

// consumes an array of strings and returns nothing
typedef void (*builtin_executor)(char *[]);

// builtin commands
struct shell_builtin
{
    char *command;             // the name of the command
    builtin_executor executor; // the function that contains the definition
};

// allocate a return a pointer to a shell_builtin populated with COMMAND and EXECUTOR
struct shell_builtin *new_builtin(char *command, builtin_executor executor);

// initialize the stack allocated builtin list
void setup_builtins();

// release all builtin structs
void cleanup_builtins();

// execute a builtin if ARGUMENTS[0] matches a builtin
// returns 1 if a builtin was found, otherwise 0
int try_exec_builtin(char *arguments[]);
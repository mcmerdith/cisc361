#pragma once

#define PrintStatus(command) printf("Executing built-in command [%s]\n", command)
#define TooFewArgs(command) printf("%s: Too few arguments.\n", command)

typedef void (*builtin_executor)(char *[]);

// linked-list of builtin commands
struct shell_builtin
{
    char *command;
    builtin_executor executor;
    struct shell_builtin *next;
};

struct shell_builtin *new_builtin(char *command, builtin_executor executor);
void link_builtins(struct shell_builtin *node, ...);

void setup_builtins();
void cleanup_builtins();
int try_exec_builtin(char *arguments[]);
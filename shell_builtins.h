#pragma once

#define PrintStatus(command) printf("Executing built-in command [%s]", command)

typedef void (*builtin_executor)(char *[]);

struct shell_builtin
{
    char *command;
    builtin_executor executor;
    struct shell_builtin *next;
};

void setup_builtins();
int try_exec_builtin(char *arguments[]);
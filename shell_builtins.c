#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "shell_builtins.h"
#include "search_path.h"

struct shell_builtin *builtins;

void _pwd(char *arguments[])
{
    char *temp = getcwd(NULL, 0);
    printf("%s\n", temp);
    free(temp); // avoid memory leak
}

void _which(char *arguments[])
{
    // which needs a command to find
    if (arguments[1] == NULL)
    {
        printf("which: Too few arguments.\n");
        return;
    }

    char *cmd = exec_which(arguments[1]);

    // Check if we found a command
    if (cmd)
    {
        printf("%s\n", cmd);
        free(cmd);
    }
    else
    {
        printf("%s: Command not found\n", arguments[1]);
    }
}

void setup_builtins()
{
    struct shell_builtin
        *pwd,
        *which,
        *where;

    // Create elements
    pwd->command = "pwd";
    pwd->executor = &_pwd;
    which->command = "which";
    which->executor = &_which;
    where->command = "where";
    where->executor = &_which;

    // Link the list
    pwd->next = which;
    which->next = where;
    where->next = NULL;

    // update head
    builtins = pwd;
}

int try_exec_builtin(char *arguments[])
{

    struct shell_builtin *current = builtins;

    while (current != NULL)
    {
        if (strcmp(current->command, arguments[0]) != 0)
            continue;

        PrintStatus(arguments[0]);

        current->executor(arguments);

        return 1;
    }

    return 0;
}
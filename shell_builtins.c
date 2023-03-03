#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "shell_builtins.h"
#include "search_path.h"

void _printStatus(char *command)
{
    printf("Executing built-in command [%s]", command);
}

int processBuiltins(char *arguments[])
{
    char *temp;

    if (strcmp(arguments[0], "pwd") == 0) // built-in command pwd
    {
        temp = getcwd(NULL, 0);
        printf("%s\n", temp);
        free(temp); // avoid memory leak
        return 1;
    }
    else if (strcmp(arguments[0], "which") == 0) // built-in command which
    {
        // which needs a command to find
        if (arguments[1] == NULL)
        {
            printf("which: Too few arguments.\n");
            return 1;
        }

        char *cmd = execWhich(arguments[1]);

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

        return 1;
    }

    return 0;
}
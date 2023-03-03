#include "sh.h"

void builtinWhich(char *arg)
{
    char *cmd = execWhich(arg);

    // Check if we found a command
    if (cmd)
    {
        printf("%s\n", cmd);
        free(cmd);
    }
    else
    {
        printf("%s: Command not found\n", arg);
    }
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

        builtinWhich(arguments[1]);
        return 1;
    }

    return 0;
}
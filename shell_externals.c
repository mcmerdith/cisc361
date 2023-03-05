#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <glob.h>
#include "shell_externals.h"
#include "search_path.h"

// expand wildcards in arguments into out_execargs.
// returns a pointer to a NULL terminated array with at most MAX_ARGS elements including the NULL-terminator
int expand_wildcards(char *arguments[], char *execargs[], int max_args)
{
    // char **execargs = *out_execargs;

    glob_t globPaths; // an array of aguments for execve()
    int csource,      // result of glob
        i, j;         // current arguments and execargs index
    char **p,         // pointer to current glob item
        *current;     // The current argument being processed

    execargs[0] = malloc(strlen(arguments[0]) + 1); // first arg is progrma name
    strcpy(execargs[0], arguments[0]);              // copy command

    j = 1;
    for (i = 1; j < max_args - 1 && (current = arguments[i]) != NULL; i++)
    { // check arguments
        if (strchr(current, '*') != NULL)
        { // wildcard!
            csource = glob(current, 0, NULL, &globPaths);
            if (csource == 0)
            {
                for (p = globPaths.gl_pathv; j < max_args - 1 && *p != NULL; ++p)
                {
                    execargs[j] = malloc(strlen(*p) + 1);
                    strcpy(execargs[j], *p);
                    j++;
                }

                globfree(&globPaths);
            }
        }
        else
        { // normal argument
            execargs[j] = malloc(strlen(arguments[i]) + 1);
            strcpy(execargs[j], arguments[i]);
            j++;
        }
    }

    // null terminate the array
    execargs[j] = NULL;

    // Check if the command to execute actually exists
    if (access(execargs[0], X_OK) != 0)
    {
        // if not, try to find it
        char *temp = exec_which(execargs[0]);

        if (temp)
        {
            free(execargs[0]);  // cleanup the old arg
            execargs[0] = temp; // assign the new one
        }
        else
        {
            printf("%s: Command not found\n", execargs[0]);
            return 0;
        }
    }

    return 1;
}
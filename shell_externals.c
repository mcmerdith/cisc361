#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <glob.h>
#include "shell_externals.h"
#include "search_path.h"

// expand wildcards in arguments into out_execargs.
// returns a pointer to a NULl terminated array
char **process_external(char *arguments[])
{
    printf("Executing external command [%s]\n", arguments[0]);

    char **execargs = malloc((MAXARGS + 1) * sizeof(char *)); // Allocate an array one larger to accommodate the NULL terminator

    glob_t globPaths; // an array of aguments for execve()
    int csource,      // result of glob
        j;            // current execargs index
    char **p,         // pointer to current glob item
        current;      // The current argument being processed

    execargs[0] = malloc(strlen(arguments[0]) + 1); // first arg is progrma name
    strcpy(execargs[0], arguments[0]);              // copy command

    j = 1;
    for (int i = 1; j < MAXARGS && (current = arguments[i]) != NULL; i++)
    { // check arguments
        if (strchr(current, '*') != NULL)
        { // wildcard!
            csource = glob(current, 0, NULL, &globPaths);
            if (csource == 0)
            {
                for (p = globPaths.gl_pathv; j < MAXARGS && *p != NULL; ++p)
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
            execargs[0] = temp;
        }
        else
        {
            printf("%s: Command not found\n", execargs[0]);
        }
    }
}
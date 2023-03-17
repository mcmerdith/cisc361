#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <glob.h>
#include "argument_util.h"
#include "search_path.h"

int expand_n_wildcards(char *arguments[], char *execargs[], int max_args)
{
    // char **execargs = *out_execargs;

    glob_t globPaths; // an array of aguments for execve()
    int csource,      // result of glob
        i, j;         // current arguments and execargs index
    char **p,         // pointer to current glob item
        *current;     // The current argument being processed

    execargs[0] = calloc(strlen(arguments[0]) + 1, sizeof(char)); // first arg is progrma name
    strcpy(execargs[0], arguments[0]);                            // copy command

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
                    execargs[j] = calloc(strlen(*p) + 1, sizeof(char));
                    strcpy(execargs[j], *p);
                    j++;
                }

                globfree(&globPaths);
            }
        }
        else
        { // normal argument
            execargs[j] = calloc(strlen(arguments[i]) + 1, sizeof(char));
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

void str_split_n(char *buffer, char *delimiter, char *arguments[], int max_args)
{
    int i;
    char *saveptr;
    arguments[0] = strtok_r(buffer, delimiter, &saveptr);
    for (i = 1; (arguments[i] = strtok_r(NULL, delimiter, &saveptr)) != NULL && i < max_args - 1; ++i)
        ;
    arguments[i] = NULL; // make sure that last element is null
}

char *join_array(char *array[], char *delimiter)
{
    // rejoin the arguments

    char *joined_string = calloc(strlen(array[0]) + 1, sizeof(char)),
         *temp;
    strcpy(joined_string, array[0]);

    for (char **p = &array[1]; *p != NULL; ++p)
    {
        temp = calloc(strlen(joined_string) + strlen(*p) + strlen(delimiter) + 1, sizeof(char)); // allocate a buffer to do the joining
        strcpy(temp, joined_string);                                                             // copy the current string into temp;
        strcat(temp, delimiter);                                                                 // space
        strcat(temp, *p);                                                                        // new segment
        free(joined_string);                                                                     // free previous segment
        joined_string = temp;                                                                    // update new segment
    }

    return joined_string;
}

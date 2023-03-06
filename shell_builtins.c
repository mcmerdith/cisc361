#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include "shell_builtins.h"
#include "defines.h"
#include "search_path.h"

struct shell_builtin *builtins[BUILTINCOUNT];

extern char *prompt_prefix;

// copies PREFIX to PROMPT_PREFIX, set to null if PREFIX is null
void _update_prefix(char *prefix)
{
    free(prompt_prefix);
    if (prefix == NULL)
    {
        prompt_prefix = NULL;
    }
    else
    {
        prompt_prefix = calloc(strlen(prefix) + 1, sizeof(char));
        strcpy(prompt_prefix, prefix);
    }
}

char *previous_dir;

#pragma region Executors

// exit the shell
void _exit_cmd(char *arguments[])
{
    exit(1);
}

// print the executable path matching arguments[1]
void _which_cmd(char *arguments[])
{
    // which needs a command to find
    if (arguments[1] == NULL)
    {
        TooFewArgs("which");
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

// print all executable paths matching arguments[1]
void _where_cmd(char *arguments[])
{
    // which needs a command to find
    if (arguments[1] == NULL)
    {
        TooFewArgs("where");
        return;
    }

    exec_where(arguments[1]);
}

// change the shell working directory
void _chdir_cmd(char *arguments[])
{
    // temp variables for holding current paths
    char *tmp_target, *tmp_cwd;

    if (arguments[1] == NULL) // cd to HOME
    {
        tmp_target = "~";
    }
    else if (strcmp(arguments[1], "-") == 0) // cd to previous directory
    {
        if (!previous_dir)
        { // Ensure there is a previous directory
            printf("chdir: no previous directory\n");
            return;
        }

        tmp_target = calloc(strlen(previous_dir) + 1, sizeof(char)); // allocate memory for the new target
        strcpy(tmp_target, previous_dir);                            // copy the previous dir into the target
    }
    else
    {
        if (access(arguments[1], R_OK) != 0)
        { // Ensure we have access to the directory
            perror("chdir");
            return;
        }

        tmp_target = calloc(strlen(arguments[1]) + 1, sizeof(char));
        strcpy(tmp_target, arguments[1]);
    }

    tmp_cwd = getcwd(NULL, 0); // get our current directory

    if (chdir(tmp_target) != 0)
    { // error state
        perror("chdir");
    }
    else
    { // success, update our previous dir
        previous_dir = reallocarray(previous_dir, strlen(tmp_cwd) + 1, sizeof(char));
        strcpy(previous_dir, tmp_cwd);
    }

    // free temp pointers
    free(tmp_target);
    free(tmp_cwd);
}

// print the current working directory
void _pwd_cmd(char *arguments[])
{
    char *temp = getcwd(NULL, 0);
    printf("%s\n", temp);
    free(temp); // avoid memory leak
}

// change the shell working directory
void _list_cmd(char *arguments[])
{
    printf("%s: Not implemented", arguments[0]);
}

// change the shell working directory
void _pid_cmd(char *arguments[])
{
    printf("%s: Not implemented", arguments[0]);
}

// change the shell working directory
void _kill_cmd(char *arguments[])
{
    printf("%s: Not implemented", arguments[0]);
}

// update the shell prompt prefix
void _prompt_cmd(char *arguments[])
{ // TODO use strncat instead of strcat
    if (arguments[1] == NULL)
    { // reset prompt
        printf("Enter a new prompt: ");
        char buffer[MAXLINE];
        if (fgets(buffer, MAXLINE, stdin) == NULL)
        {
            printf("prompt: not updated\n");
            clearerr(stdin);
            return;
        }

        // Newlines are unnecessary
        if (buffer[strlen(buffer) - 1] == '\n')
            buffer[strlen(buffer) - 1] = 0; // replace newline with null

        if (strlen(buffer) == 0)
        { // If still empty reset
            _update_prefix(NULL);
        }
        else
        { // Otherwise update
            _update_prefix(buffer);
        }

        return;
    }

    // rejoin the arguments

    char *new_prefix = calloc(strlen(arguments[1]) + 1, sizeof(char)),
         *temp;
    strcpy(new_prefix, arguments[1]);

    for (char **p = &arguments[2]; *p != NULL; ++p)
    {
        temp = calloc(strlen(new_prefix) + strlen(*p) + 2, sizeof(char)); // allocate a buffer to do the joining
        strcpy(temp, new_prefix);                                         // copy the current string into temp;
        strcat(temp, " ");                                                // space
        strcat(temp, *p);                                                 // new segment
        free(new_prefix);                                                 // free previous segment
        new_prefix = temp;                                                // update new segment
    }

    _update_prefix(new_prefix); // Set the new prefix
    free(new_prefix);           // Free our prefix
}

// change the shell working directory
void _printenv_cmd(char *arguments[])
{
    printf("%s: Not implemented", arguments[0]);
}

// change the shell working directory
void _setenv_cmd(char *arguments[])
{
    printf("%s: Not implemented", arguments[0]);
}

#pragma endregion

#pragma region Initialization

// allocate memory for a builtin
inline struct shell_builtin *new_builtin(char *command, builtin_executor executor)
{
    struct shell_builtin *new_builtin = malloc(sizeof(struct shell_builtin));
    new_builtin->command = calloc(strlen(command) + 1, sizeof(char));
    strcpy(new_builtin->command, command);
    new_builtin->executor = executor;
    return new_builtin;
}

// allocate and populate a linked-list of builtins
void setup_builtins()
{
    struct shell_builtin *tmp[BUILTINCOUNT] = {new_builtin("exit", &_exit_cmd),
                                               new_builtin("which", &_which_cmd),
                                               new_builtin("where", &_where_cmd),
                                               new_builtin("cd", &_chdir_cmd),
                                               new_builtin("pwd", &_pwd_cmd),
                                               new_builtin("list", &_list_cmd),
                                               new_builtin("pid", &_pid_cmd),
                                               new_builtin("kill", &_kill_cmd),
                                               new_builtin("prompt", &_prompt_cmd),
                                               new_builtin("printenv", &_printenv_cmd),
                                               new_builtin("setenv", &_setenv_cmd)};

    for (int i = 0; i < BUILTINCOUNT; ++i)
        builtins[i] = tmp[i];
}

void cleanup_builtins()
{
    for (int i = 0; i < BUILTINCOUNT; ++i)
    {
        free(builtins[i]->command);
        free(builtins[i]);
    }
}

#pragma endregion

int try_exec_builtin(char *arguments[])
{
    for (int i = 0; i < BUILTINCOUNT; ++i)
    {
        if (strcmp(builtins[i]->command, arguments[0]) != 0)
            continue;

        PrintStatus(arguments[0]);

        builtins[i]->executor(arguments);

        return 1;
    }

    return 0;
}
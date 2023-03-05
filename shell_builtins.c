#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include "shell_builtins.h"
#include "search_path.h"

struct shell_builtin *builtins;

extern char *prompt_prefix;

#pragma region Executors

// exit the shell
void _exit_cmd(char *arguments[])
{
    exit(0);
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
    printf("%s: Not implemented", arguments[1]);
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
    printf("%s: Not implemented", arguments[1]);
}

// change the shell working directory
void _pid_cmd(char *arguments[])
{
    printf("%s: Not implemented", arguments[1]);
}

// change the shell working directory
void _kill_cmd(char *arguments[])
{
    printf("%s: Not implemented", arguments[1]);
}

// update the shell prompt prefix
void _prompt_cmd(char *arguments[])
{
    if (arguments[1] == NULL)
    {
        TooFewArgs("prompt");
        return;
    }

    // rejoin the arguments

    char *dest = malloc(sizeof(char) * (strlen(arguments[1]) + 1)), *temp;
    strcpy(dest, arguments[1]);
    for (char **p = &arguments[2]; *p != NULL; ++p)
    {
        temp = malloc(sizeof(char) * (strlen(dest) + strlen(*p) + 2)); // allocate a buffer to do the joining
        // sprintf(temp, "%s %s", dest, *p);
        strcpy(temp, dest); // copy the current string into temp;
        strcat(temp, " ");  // space
        strcat(temp, *p);   // new segment
        free(dest);         // free previous segment
        dest = temp;        // update new segment
    }

    free(prompt_prefix);
    prompt_prefix = dest;
}

// change the shell working directory
void _printenv_cmd(char *arguments[])
{
    printf("%s: Not implemented", arguments[1]);
}

// change the shell working directory
void _setenv_cmd(char *arguments[])
{
    printf("%s: Not implemented", arguments[1]);
}

#pragma endregion

#pragma region Memory Management for linkedlist

// allocate memory for a builtin
inline struct shell_builtin *new_builtin(char *command, builtin_executor executor)
{
    struct shell_builtin *new_builtin = malloc(sizeof(struct shell_builtin));
    new_builtin->command = command;
    new_builtin->executor = executor;
    return new_builtin;
}

// link some builtins together
void link_builtins(struct shell_builtin *head, ...)
{
    va_list argp;
    va_start(argp, head);

    struct shell_builtin *current = head,
                         *previous;

    while (current != NULL)
    {
        previous = current;
        current = va_arg(argp, struct shell_builtin *);
        previous->next = current;
    }
}

// allocate and populate a linked-list of builtins
void setup_builtins()
{
    struct shell_builtin
        *pwd = new_builtin("pwd", &_pwd_cmd),
        *which = new_builtin("which", &_which_cmd),
        *where = new_builtin("where", &_where_cmd),
        *exit = new_builtin("exit", &_exit_cmd),
        *prompt = new_builtin("prompt", &_prompt_cmd);

    // Link the list
    link_builtins(pwd, which, where, exit, prompt);

    // update head
    builtins = pwd;
}

// Free the linked-list of builtins
void cleanup_builtins()
{
    struct shell_builtin
        *current = builtins,
        *temp;

    // free each element of the linked-list
    while (current != NULL)
    {
        temp = current;
        current = current->next;
        free(temp->command);
        free(temp);
    }
}

#pragma endregion

int try_exec_builtin(char *arguments[])
{

    struct shell_builtin *current = builtins;

    while (current != NULL)
    {
        if (strcmp(current->command, arguments[0]) != 0)
        {
            current = current->next;
            continue;
        }

        PrintStatus(arguments[0]);

        current->executor(arguments);

        return 1;
    }

    return 0;
}
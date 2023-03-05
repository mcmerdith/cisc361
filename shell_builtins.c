#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "shell_builtins.h"
#include "search_path.h"

struct shell_builtin *builtins;

extern char *prompt_prefix;

#pragma region Executors

// print the current working directory
void _pwd(char *arguments[])
{
    char *temp = getcwd(NULL, 0);
    printf("%s\n", temp);
    free(temp); // avoid memory leak
}

// print the executable path matching arguments[1]
void _which(char *arguments[])
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
void _where(char *arguments[])
{
    // which needs a command to find
    if (arguments[1] == NULL)
    {
        TooFewArgs("where");
        return;
    }

    exec_where(arguments[1]);
}

// exit the shell
void _exit_cmd(char *arguments[])
{
    exit(0);
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

#pragma endregion

#pragma region Memory Management for linked -list

// allocate memory for a builtin
struct shell_builtin *_new_builtin()
{
    return malloc(sizeof(struct shell_builtin));
}

// allocate and populate a linked-list of builtins
void setup_builtins()
{
    struct shell_builtin
        *pwd = _new_builtin(),
        *which = _new_builtin(),
        *where = _new_builtin(),
        *exit = _new_builtin(),
        *prompt = _new_builtin();

    // Create elements
    pwd->command = "pwd";
    pwd->executor = &_pwd;
    which->command = "which";
    which->executor = &_which;
    where->command = "where";
    where->executor = &_where;
    exit->command = "exit";
    exit->executor = &_exit_cmd;
    prompt->command = "prompt";
    prompt->executor = &_prompt_cmd;

    // Link the list
    pwd->next = which;
    which->next = where;
    where->next = exit;
    exit->next = prompt;
    prompt->next = NULL;

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
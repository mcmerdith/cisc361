#pragma once

typedef struct redirection_node
{
    char *filename;
    int b_append,
        b_also_err;
    struct redirection_node *next_node;
} redirection_node;

typedef struct shell_command
{
    int b_background;               // if this command will run in the background. backgrounded jobs cannot be piped
    char *command;                  // the command to be executed
    int pipefd[2];                  // pipe is read before redirections
    redirection_node *rdin, *rdout; // input is read before output is written
} shell_command;

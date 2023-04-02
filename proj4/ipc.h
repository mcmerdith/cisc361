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

// free's all related resources for COMMAND
void free_command(shell_command *command);

// write the contents of all files in the linked-list defined by HEAD into WRITE_FD
// returns 1 on success, 0 if a file error occurs
int redirect_input(redirection_node *head, int write_fd);

// write the contents of READ_FD to all files in the linked-list defined by HEAD
// blocking operation, function will not return until READ_FD is closed
// return 1 on success, 0 if a file error occurs
int redirect_output_worker(redirection_node *head, int read_fd);
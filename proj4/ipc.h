#pragma once

typedef struct redirection_node
{
    char *filename; // the name of the file to open, NULL if the fd is already initialized
    int fd;         // the open file descriptor, -1 if a file is not open
    int b_append,   // if the redirection should be append
        b_also_err; // if stderr should also be redirected
    struct redirection_node *next_node;
} redirection_node;

typedef struct shell_command
{
    int b_background;               // if this command will run in the background. backgrounded jobs cannot be piped
    char *command;                  // the command to be executed
    redirection_node *rdin, *rdout; // input is read before output is written
    struct shell_command *next_node;
} shell_command;

// free's all related resources for COMMAND
void free_commands(shell_command *command);

// write the contents of all files in the linked-list defined by HEAD into WRITE_FD
// returns 1 on success, 0 if a file error occurs
int redirect_input(redirection_node *head, int write_fd);

// write the contents of READ_FD to all files in the linked-list defined by HEAD
// blocking operation, function will not return until READ_FD is closed
// return 1 on success, 0 if a file error occurs
int redirect_output_worker(redirection_node *head, int read_fd);
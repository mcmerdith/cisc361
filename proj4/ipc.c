#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "ipc.h"
#include "defines.h"

void _free_redirections(redirection_node *head)
{
    redirection_node *temp, *current = head;
    while (current != NULL)
    {
        temp = current;
        current = current->next_node;
        free(temp->filename);
        free(temp);
    }
}

void free_commands(shell_command *command)
{
    shell_command *temp, *curr = command;
    while (curr != NULL)
    {
        temp = curr;
        curr = curr->next_node;
        _free_redirections(temp->rdin);
        _free_redirections(temp->rdout);
        free(temp->command);
        free(temp);
    }
}

int redirect_input(redirection_node *head, int write_fd)
{
    int file, size;
    char databuff[MAXLINE];

    for (redirection_node *curr = head; curr != NULL; curr = curr->next_node)
    {
        if (0 > (file = curr->fd) && 0 > (file = open(curr->filename, O_RDONLY))) // try to read the file
        {
            char message[MAXLINE];                                            // some space for the message
            snprintf(message, MAXLINE, "error reading '%s'", curr->filename); // make the message
            perror(message);                                                  // oops
            continue;                                                         // skip this file
        }

        while (0 < (size = read(file, databuff, MAXLINE)))
        {
            write(write_fd, databuff, size);
        }

        close(file); // we done
    }

    return 1;
}

void _close_open_files(redirection_node *head_file)
{
    for (redirection_node *curr_file = head_file; curr_file != NULL;)
    {
        close(curr_file->fd);
        // temp = curr_file;
        curr_file = curr_file->next_node;
        // free(temp);
    }
}

int redirect_output_worker(redirection_node *head, int read_fd)
{
    redirection_node **curr_file = &head;

    /* Open all the files */

    for (redirection_node *curr = head; curr != NULL; curr = curr->next_node)
    {
        if ((*curr_file)->fd < 0)
        {
            if (access(curr->filename, W_OK) < 0)
            { // no file
                (*curr_file)->fd = creat(curr->filename, S_IRWXU | S_IRGRP | S_IROTH);
            }
            else
            { // yay, file
                // todo: noclobber
                (*curr_file)->fd = open(curr->filename, O_WRONLY);
            }

            if ((*curr_file)->fd < 0)
            {
                perror("error writing file"); // oops
                _close_open_files(head);      // release resources
                return 0;                     // byeybe
            }
        }

        curr_file = &(*curr_file)->next_node;
    }

    *curr_file = NULL; // put *something* in the last spot

    /* Read sections from the input into all the files until its closed */

    char buff[MAXLINE];
    int readsize;
    while (0 < (readsize = read(read_fd, buff, MAXLINE)))
    {
        for (curr_file = &head; *curr_file != NULL; curr_file = &(*curr_file)->next_node)
            write((*curr_file)->fd, buff, readsize);
    }

    _close_open_files(head);

    return 1; // we done
}
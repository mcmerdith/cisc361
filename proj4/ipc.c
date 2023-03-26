#include <stdlib.h>
#include "ipc.h"

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

void free_command(shell_command *command)
{
    _free_redirections(command->rdin);
    _free_redirections(command->rdout);
    free(command->command);
    free(command);
}
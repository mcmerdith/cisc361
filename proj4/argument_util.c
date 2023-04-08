#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <glob.h>
#include <ctype.h>
#include "argument_util.h"
#include "defines.h"
#include "search_path.h"

int expand_n_wildcards(char *arguments[], char *execargs[], int max_args)
{
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

void str_split_n(char *buffer, char *delimiter, char *out_segments[], int max_args)
{
    int i;
    char *saveptr;
    out_segments[0] = strtok_r(buffer, delimiter, &saveptr);
    for (i = 1; (out_segments[i] = strtok_r(NULL, delimiter, &saveptr)) != NULL && i < max_args - 1; ++i)
        ;
    out_segments[i] = NULL; // make sure that last element is null
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

void trim_whitespace(char *input)
{
    int start, end;

    for (int i = 0; i < strlen(input) + 1; ++i) // check every character including null terminator
    {
        if (input[i] == 0) // end of string
        {
            input = 0;
            return;
        }

        if (!isspace(input[i]))
        { // beginning of non-whitespace
            start = i;
            break;
        }
    }

    for (int i = strlen(input) - 1; i >= 0; --i) // check every character except null terminator in reverse
    {
        if (!isspace(input[i]))
        { // end of non-whitespace
            end = i;
            break;
        }
    }

    memmove(input, input + start, end - start + 1); // move the string
    input[end - start + 1] = 0;                     // null-terminate
}

int array_len(void *array[], int max_len)
{
    int i = 0;
    for (; array[i] != NULL && i < max_len; ++i)
        ;
    return i;
}

// LINKED_LIST is the redirection chain to attach to, FOUND_SYMBOL is the redirector that was found
// CURRENT_COMMAND is the current section of the command to parse, and TARGET is where to write the found segment before
// technically this will allow <&, <<, and <<&, but that can be filtered later
void _assign_redirection(redirection_node **linked_list, char *found_symbol, char **current_command, char ***target)
{
    redirection_node *current = malloc(sizeof(redirection_node));
    current->fd = -1;
    current->next_node = *linked_list; // next is current head
    *linked_list = current;            // current is new head

    char redir_type = *found_symbol;

    *found_symbol = 0; // swap with a terminator
    ++found_symbol;    // move to the next character

    if (redir_type == *found_symbol) // if we found a second instance
    {
        current->b_append = 1; // >> means out append
        ++found_symbol;        // skip this character
    }
    else
    {
        current->b_append = 0; // > means out
    }

    if ('&' == *found_symbol)
    {
        current->b_also_err = 1; // & also does stderr
        ++found_symbol;          // skip this character
    }
    else
    {
        current->b_also_err = 0;
    }

    **target = calloc(strlen(*current_command) + 1, sizeof(char)); // allocate the buffer
    strcpy(**target, *current_command);                            // copy the current element
    trim_whitespace(**target);                                     // trim the whitespace

    *target = &(current->filename);  // the next target will be our current filename
    *current_command = found_symbol; // the next element will start after the current one
}

shell_command *_parse_shell_command(char *command)
{
    shell_command *current_command = malloc(sizeof(shell_command));

    char *final = command + strlen(command) - 1;
    if ('&' == *final)
    {
        current_command->b_background = 1; // set the flag
        *final = 0;                        // get rid of it
        trim_whitespace(command);          // remove any leftover whitespace
    }
    else
    {
        current_command->b_background = 0; // not a background command
    }

    current_command->rdin = NULL;  // no redireciton by default
    current_command->rdout = NULL; // no redirection by default

    char *start = command, *found_redirector = strpbrk(start, "<>"); // check if we have any redirections

    if (found_redirector == NULL) // no redirection
    {
        current_command->command = calloc(strlen(command) + 1, sizeof(char));
        strcpy(current_command->command, command);
    }
    else // redirection
    {
        char **target = &(current_command->command);

        redirection_node **curr_head;

        while (found_redirector != NULL) // while we have more redirections continually loop
        {
            curr_head = ('>' == *found_redirector) ? &(current_command->rdout) : &(current_command->rdin); // select which list to redirect to
            _assign_redirection(curr_head, found_redirector, &start, &target);                             // recursively process the redirections
            found_redirector = strpbrk(start, "<>");                                                       // go again
        }

        // copies the last element
        *target = calloc(strlen(start) + 1, sizeof(char)); // allocate the buffer
        strcpy(*target, start);                            // copy the current element
        trim_whitespace(*target);                          // trim the whitespace
    }

    return current_command;
}

void _link_pipes(shell_command *head)
{
    redirection_node *in, *out;
    int pipefd[2];

    for (shell_command *curr = head; curr != NULL; curr = curr->next_node)
    {
        if (curr->next_node)
        {
            in = malloc(sizeof(redirection_node));
            out = malloc(sizeof(redirection_node));

            pipe(pipefd); // create the pipe

            in->filename = out->filename = NULL; // signifies that the filedescriptor is already open
            in->b_append = out->b_append = 0;    // IPC doesn't append

            in->fd = pipefd[READ_END];   // input process receives the read end
            out->fd = pipefd[WRITE_END]; // output process receives the write end

            if ('&' == *curr->next_node->command) // STDERR redirection
            {
                curr->next_node->command = memmove(curr->next_node->command,
                                                   curr->next_node->command + 1,
                                                   strlen(curr->next_node->command)); // remove the &
                trim_whitespace(curr->next_node->command);                            // trim whitespace
                in->b_also_err = out->b_also_err = 1;                                 // set the flag
            }
            else
            {
                in->b_also_err = out->b_also_err = 0; // no STDERR
            }

            // stitch the elements into their respective lists
            out->next_node = curr->rdout;
            curr->rdout = out;

            in->next_node = curr->next_node->rdin;
            curr->next_node->rdin = in;
        }
    }
}

shell_command *parse_commands(char *buffer)
{
    // Figure out what is happening with the pipes and redirections

    char *expanded[MAXARGS];
    str_split_n(buffer, "|", expanded, MAXARGS); // split the commands at the pipes (leaves behind the & for the parser)

    shell_command *head, **current = &head;
    for (int i = 0; expanded[i] != NULL; ++i)
    {                                                 // each piped command (still has whitespace)
        trim_whitespace(expanded[i]);                 // remove said whitespace
        *current = _parse_shell_command(expanded[i]); // parse the command
        current = &(*current)->next_node;             // move the pointer
    }

    *current = NULL; // put something in the last spot

    _link_pipes(head); // link the piped processes together

    return head;
}

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "search_path.h"

#define PATHLEN 128

// Search the path P for COMMAND
// if bPrintOrReturn, prints all matching commands and returns NULL
// otherwise, returns the first matching path
char *path_search(char *command, struct pathelement *p, int bPrintOrReturn)
{
  char cmd[PATHLEN], *ch = NULL;

  while (p)
  {
    sprintf(cmd, "%s/%s", p->element, command);
    if (access(cmd, X_OK) == 0)
    {
      if (bPrintOrReturn)
      {
        printf("%s\n", cmd);
      }
      else
      {
        // return the path
        ch = calloc(strlen(cmd) + 1, sizeof(char));
        strcpy(ch, cmd);
        break;
      }
    }
    p = p->next;
  }

  return ch;
}

// free list of path values
void free_path(struct pathelement *p)
{
  struct pathelement *tmp;

  while (p)
  {
    tmp = p;
    p = p->next;
    free(tmp->element);
    free(tmp);
  }
}

// Execute the `which` command, returning the path or NULL if not found. Caller must free the returned path
char *exec_which(char *arg)
{
  struct pathelement *p = get_path(); // the first node of the path linked list
  char *cmd = path_search(arg, p, 0); // the location of the command

  free_path(p);

  return cmd;
}

// Execute the `which` command, returning the path or NULL if not found. Caller must free the returned path
void exec_where(char *arg)
{
  struct pathelement *p = get_path(); // the first node of the path linked list
  path_search(arg, p, 1);             // the location of the command

  free_path(p);
}

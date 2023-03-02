#include "get_path.h"
#define PATHLEN 128

char *which(char *command, struct pathelement *p)
{
  char cmd[PATHLEN], *ch;
  int found;

  found = 0;
  while (p)
  {
    sprintf(cmd, "%s/%s", p->element, command);
    if (access(cmd, X_OK) == 0)
    {
      found = 1;
      break;
    }
    p = p->next;
  }
  if (found)
  {
    ch = malloc(strlen(cmd) + 1);
    strcpy(ch, cmd);
    return ch;
  }
  else
    return (char *)NULL;
}

// Execute the `which` command, returning the path or NULL if not found. Caller must free the returned path
char *execWhich(char *arg)
{
  struct pathelement *p = get_path(), // the first node of the path linked list
      *tmp;                           // pointers to our path
  char *cmd = which(arg, p);          // the location of the command

  // free list of path values
  while (p)
  {
    tmp = p;
    p = p->next;
    free(tmp->element);
    free(tmp);
  }

  return cmd;
}

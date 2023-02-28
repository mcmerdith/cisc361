#include "get_path.h"
#define PATHLEN 128

char *which(char *command, struct pathelement *p)
{
  char cmd[PATHLEN], *ch;
  int  found;

  found = 0;
  while (p) {       
    sprintf(cmd, "%s/%s", p->element, command);
    if (access(cmd, X_OK) == 0) {
      found = 1;
      break;
    }
    p = p->next;
  }
  if (found) {
    ch = malloc(strlen(cmd)+1);
    strcpy(ch, cmd);
    return ch;
  }
  else
    return (char *) NULL;
}

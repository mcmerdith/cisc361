#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <glob.h>
#include <sys/wait.h>
#include "sh.h"

void prompt(int bPrintNewline)
{
  fprintf(stdout, "%s>> ", bPrintNewline ? "\n" : "");
  fflush(stdout);
}

void sig_handler(int sig)
{
  prompt(1);
}

int main(int argc, char **argv, char **envp)
{
  char buffer[MAXLINE],    // temporary buffer for fgets
      *arguments[MAXARGS], // an array of tokens
      *temp,               // use whenever a string needs to be temporarily stored
      *argumentParts;      // used for argument parsing
  pid_t pid;               // pid of the executed command
  int status,              // status of the executed command
      i,                   // iterators and stuff
      argumentCount;       // # of tokens in command line

  signal(SIGINT, sig_handler);

  prompt(0);

  while (fgets(buffer, MAXLINE, stdin) != NULL)
  {
    // Print the prompt
    prompt(0);

    // Skip processing if no command was entered
    if (strlen(buffer) == 1 && buffer[strlen(buffer) - 1] == '\n')
      continue;

    // Newlines will confuse the parser
    if (buffer[strlen(buffer) - 1] == '\n')
      buffer[strlen(buffer) - 1] = 0; // replace newline with null

    // parse command line into tokens (stored in buffer)
    argumentCount = 0;
    argumentParts = strtok(buffer, " ");
    while (argumentParts != NULL && argumentCount < MAXARGS)
    {
      arguments[argumentCount] = argumentParts;
      argumentCount++;
      argumentParts = strtok(NULL, " ");
    }
    arguments[argumentCount] = (char *)NULL;

    if (arguments[0] == NULL) // "blank" command line with SPACES
      continue;

    /* print tokens
    for (i = 0; i < argumentCount; i++)
      printf("arg[%d] = %s\n", i, arg[i]);
    */

    if (strcmp(arguments[0], "pwd") == 0)
    { // built-in command pwd
      printf("Executing built-in [pwd]\n");
      temp = getcwd(NULL, 0);
      printf("%s\n", temp);
      free(temp); // avoid memory leak
    }
    else if (strcmp(arguments[0], "which") == 0)
    { // built-in command which
      struct pathelement *p, *tmp;
      char *cmd;

      printf("Executing built-in [which]\n");

      if (arguments[1] == NULL)
      { // "empty" which
        printf("which: Too few arguments.\n");
        continue;
      }

      p = get_path();

      /***/
      tmp = p;
      while (tmp)
      { // print list of paths
        printf("path [%s]\n", tmp->element);
        tmp = tmp->next;
      }
      /***/

      cmd = which(arguments[1], p);
      if (cmd)
      {
        printf("%s\n", cmd);
        free(cmd);
      }
      else // argument not found
        printf("%s: Command not found\n", arguments[1]);

      while (p)
      { // free list of path values
        tmp = p;
        p = p->next;
        free(tmp->element);
        free(tmp);
      }
    }
    else
    { // external commands
      if ((pid = fork()) < 0)
      {
        printf("fork error");
      }
      else if (pid == 0)
      { // child
        // an array of aguments for execve()
        char *execargs[MAXARGS];
        glob_t paths;
        int csource, j;
        char **p;

        execargs[j] = malloc(strlen(arguments[0]) + 1);
        strcpy(execargs[0], arguments[0]); // copy command
        j = 1;
        for (i = 1; i < argumentCount; i++)
        { // check arguments
          if (strchr(arguments[i], '*') != NULL)
          { // wildcard!
            csource = glob(arguments[i], 0, NULL, &paths);
            if (csource == 0)
            {
              for (p = paths.gl_pathv; *p != NULL; ++p)
              {
                execargs[j] = malloc(strlen(*p) + 1);
                strcpy(execargs[j], *p);
                j++;
              }

              globfree(&paths);
            }
          }
          else
          {
            execargs[j] = malloc(strlen(arguments[i]) + 1);
            strcpy(execargs[j], arguments[i]);
            j++;
          }
        }

        execargs[j] = NULL;

        i = 0;
        for (i = 0; i < j; i++)
          printf("exec arg [%s]\n", execargs[i]);

        execve(execargs[0], execargs, NULL);
        printf("couldn't execute: %s", buffer);
        exit(127);
      }
      // parent
      if ((pid = waitpid(pid, &status, 0)) < 0)
        printf("waitpid error");
      /**
            if (WIFEXITED(status)) // S&R p. 239
              printf("child terminates with (%d)\n", WEXITSTATUS(status));
      **/

    nextprompt:
    }
  }

  exit(0);
}

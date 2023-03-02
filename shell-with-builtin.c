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

void builtinWhich(char *arg)
{
  char *cmd = execWhich(arg);

  // Check if we found a command
  if (cmd)
  {
    printf("%s\n", cmd);
    free(cmd);
  }
  else
  {
    printf("%s: Command not found\n", arg);
  }
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

  while (1)
  {
    // Print the prompt
    prompt(0);

    if (fgets(buffer, MAXLINE, stdin) == NULL)
      break;

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

    // Our first argument should be the program/command. If null, we have nothing to execute
    if (arguments[0] == NULL)
      continue;

    if (strcmp(arguments[0], "pwd") == 0) // built-in command pwd
    {
      printf("Executing built-in [pwd]\n");
      temp = getcwd(NULL, 0);
      printf("%s\n", temp);
      free(temp); // avoid memory leak
    }
    else if (strcmp(arguments[0], "which") == 0) // built-in command which
    {
      // which needs a command to find
      if (arguments[1] == NULL)
      {
        printf("which: Too few arguments.\n");
        continue;
      }

      builtinWhich(arguments[1]);
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
        glob_t globPaths;
        int csource, j;
        char **p;

        execargs[0] = malloc(strlen(arguments[0]) + 1);
        strcpy(execargs[0], arguments[0]); // copy command
        j = 1;
        for (i = 1; i < argumentCount; i++)
        { // check arguments
          if (strchr(arguments[i], '*') != NULL)
          { // wildcard!
            csource = glob(arguments[i], 0, NULL, &globPaths);
            if (csource == 0)
            {
              for (p = globPaths.gl_pathv; *p != NULL; ++p)
              {
                execargs[j] = malloc(strlen(*p) + 1);
                strcpy(execargs[j], *p);
                j++;
              }

              globfree(&globPaths);
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

        execve(execargs[0], execargs, NULL);
        printf("couldn't execute: %s\n", buffer);
        exit(127);
      }
      // parent
      if ((pid = waitpid(pid, &status, 0)) < 0)
        printf("waitpid error\n");

      if (WIFEXITED(status)) // S&R p. 239
        printf("child terminates with (%d)\n", WEXITSTATUS(status));
    }
  }

  return 0;
}

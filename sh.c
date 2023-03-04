#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "sh.h"
#include "shell_builtins.h"
#include "shell_externals.h"
#include "search_path.h"

void prompt(int bPrintNewline)
{
  char *cwd = getcwd(NULL, 0);
  fprintf(stdout, "%s%s  msh > ", bPrintNewline ? "\n" : "", cwd);
  free(cwd);
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
      **execargs,          // args for execve
      *temp,               // use whenever a string needs to be temporarily stored
      *argumentParts;      // used for argument parsing
  pid_t pid;               // pid of the executed command
  int status,              // status of the executed command
      i,                   // iterators and stuff
      argumentCount;       // # of tokens in command line

  signal(SIGINT, sig_handler);

  setup_builtins();

  while (1)
  {
    // Print the prompt
    prompt(0);

    if (fgets(buffer, MAXLINE, stdin) == NULL)
    {
      printf("\nUse \"exit\" to exit msh\n");

      // Dispose of the EOF in the fgets buffer
      ungetc(1, stdin);
      getc(stdin);

      continue;
    }

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

    if (try_exec_builtin(arguments))
    {
      continue;
    }
    else
    { // external commands
      execargs = process_external(arguments);
      if ((pid = fork()) < 0)
      {
        printf("Failed to execute: fork error\n");
      }
      else if (pid == 0)
      {                                      // child
        execve(execargs[0], execargs, NULL); // if execution succeeds, child process stops here
        printf("couldn't execute: %s\n", buffer);
        exit(127);
      }

      // parent
      if ((pid = waitpid(pid, &status, 0)) < 0)
        printf("waitpid error\n");
    }
  }

  cleanup_builtins();

  return 0;
}

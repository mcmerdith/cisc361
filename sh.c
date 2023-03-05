#define __USE_POSIX

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

char *prompt_prefix = NULL;

void prompt(int bPrintNewline)
{
  char *cwd = getcwd(NULL, 0);
  if (bPrintNewline)
    printf("\n");
  if (prompt_prefix)
    printf("%s ", prompt_prefix);

  printf("[%s] msh > ", cwd);
  free(cwd);
  fflush(stdout);
}

void sig_handler(int sig)
{
  prompt(1);
  // printf("%d", getchar());
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

  struct sigaction action;
  action.sa_handler = sig_handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = SA_RESTART;

  sigaction(SIGINT, &action, NULL);
  sigaction(SIGTSTP, &action, NULL);

  setup_builtins();

  execargs = malloc((MAXARGS + 1) * sizeof(char *)); // Allocate an array one larger to accommodate the NULL terminator,

  while (1)
  {
    // Print the prompt
    prompt(0);

    if (fgets(buffer, MAXLINE, stdin) == NULL)
    {
      printf("\nUse \"exit\" to exit msh\n");

      // Clear the error
      clearerr(stdin);
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
    else if (process_n_externals(arguments, &execargs, MAXARGS))
    { // external commands
      if ((pid = fork()) < 0)
      {
        printf("Failed to execute: fork error\n");
      }
      else if (pid == 0)
      { // child
        printf("Executing [%s]\n", execargs[0]);
        execve(execargs[0], execargs, NULL); // if execution succeeds, child process stops here
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

  cleanup_builtins();

  return 0;
}

#define __USE_POSIX

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "sh.h"
#include "defines.h"
#include "shell_builtins.h"
#include "argument_util.h"
#include "search_path.h"

char *prompt_prefix = NULL;
pid_t shell_pid;

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
}

void shutdown()
{
  cleanup_builtins();
  free(prompt_prefix);
}

int main(int argc, char **argv, char **envp)
{
  printf("\nWelcome to\n");
  printf("    __  ___   _____    __  __\n");   // super
  printf("   /  |/  /  / ___/   / / / /\n");   // dope
  printf("  / /|_/ /   \\__ \\   / /_/ / \n"); // ascii
  printf(" / /  / /   ___/ /  / __  /  \n");   // art
  printf("/_/  /_/   /____/  /_/ /_/   \n");   // intro
  printf("\nv1.0\n\n");

  shell_pid = getpid();

  char buffer[MAXLINE],    // temporary buffer for fgets
      *arguments[MAXARGS]; // an array of tokens
  pid_t pid;               // pid of the executed command
  int status;              // status of the executed command

  // Set up our signal handlers
  struct sigaction action;
  action.sa_handler = sig_handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = SA_RESTART;

  sigaction(SIGINT, &action, NULL);
  sigaction(SIGTSTP, &action, NULL);
  atexit(&shutdown);

  // Setup our builtin commands
  setup_builtins();

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
    expand_n_arguments(buffer, arguments, MAXARGS);

    // Our first argument should be the program/command. If null, we have nothing to execute
    if (arguments[0] == NULL)
      continue;

    if (try_exec_builtin(arguments))
    {
      continue;
    }
    else
    { // external commands
      if ((pid = fork()) < 0)
      {
        printf("Failed to execute: fork error\n");
      }
      else if (pid == 0) // child
      {
        char *execargs[MAXARGS]; // args for execve
        if (!expand_n_wildcards(arguments, execargs, MAXARGS))
          exit(1); // Skip execution if failure
        printf("Executing [%s]\n", execargs[0]);
        execve(execargs[0], execargs, envp); // if execution succeeds, child process stops here
        printf("couldn't execute: %s\n", buffer);
        exit(127);
      }

      // parent
      if ((pid = waitpid(pid, &status, 0)) < 0)
        printf("waitpid error\n");

      if (WIFEXITED(status)) // S&R p. 239
      {
        int exitcode = WEXITSTATUS(status);
        if (exitcode != 0)
          printf("process terminated with code %d\n", exitcode);
      }
    }
  }

  shutdown();

  return 0;
}

#define __USE_POSIX

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include "sh.h"
#include "defines.h"
#include "shell_builtins.h"
#include "argument_util.h"
#include "search_path.h"

char *prompt_prefix = NULL;
pid_t shell_pid;

void prompt(int bPrintNewline)
{

  if (bPrintNewline)
    printf("\n");
  if (prompt_prefix)
    printf("%s ", prompt_prefix);

  char *cwd = getcwd(NULL, 0), *home = getenv("HOME"), *occur = strstr(cwd, home);
  if (occur != NULL && occur == cwd)                                   // checks if the cwd starts with the home directory
  {                                                                    // replace ENV[HOME] with ~
    char *temp = calloc(strlen(cwd) - strlen(home) + 1, sizeof(char)); // allocate a buffer for the cwd without homedir
    strcpy(temp, occur + strlen(home));                                // move the remainder of cwd
    cwd = reallocarray(cwd, strlen(temp) + 2, sizeof(char));           // 2 additional characters for the ~ and null-terminator
    strcpy(cwd, "~");
    strcat(cwd, temp);
    free(temp);
  }

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

void _pipe_data(int fdout, int fdin)
{
  char databuff[MAXLINE];
  int size;

  while (0 < (size = read(fdin, databuff, MAXLINE)))
  {
    write(fdout, databuff, size);
  }
}

void _stdout_redirection_worker(redirection_node *head, int read_fd)
{
  redirection_node *curr = head;
  struct cached_file
  {
    int fd;
    struct cached_file *next;
  } *head_file = malloc(sizeof(struct cached_file));

  struct cached_file **curr_file = &head_file;

  while (curr != NULL) // open all files
  {
    if (access(curr->filename, W_OK) < 0)
    { // no file
      (*curr_file)->fd = creat(curr->filename, S_IRWXU | S_IRGRP | S_IROTH);
    }
    else
    { // yay, file
      // todo: noclobber
      (*curr_file)->fd = open(curr->filename, O_WRONLY);
    }

    if ((*curr_file)->fd < 0)
    {
      perror("error writing file"); // oops
      exit(1);                      // byeybe
    }

    (*curr_file)->next = malloc(sizeof(struct cached_file));
    curr_file = &(*curr_file)->next;
  }

  char buff[MAXLINE];
  int readsize;
  while (0 < (readsize = read(read_fd, buff, MAXLINE)))
  {
    while (*curr_file != NULL)
    {
      write((*curr_file)->fd, buff, readsize);
      if ((curr_file = &(*curr_file)->next) == NULL) // next
        curr_file = &head_file;                      // or repeat if no more
    }
  }

  exit(0); // we done
}

int process_command(shell_command *command, char **envp)
{
  char *arguments[MAXARGS]; // an array of tokens
  pid_t pid;                // pid of the executed command
  int status;               // status of the executed command
  // parse command line into tokens (stored in buffer)
  str_split_n(command->command, " ", arguments, MAXARGS);

  // Our first argument should be the program/command. If null, we have nothing to execute
  if (arguments[0] == NULL)
    return 0;

  if (try_exec_builtin(arguments))
  {
    return 1;
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

      // Handle piping

      redirection_node *curr = command->rdin;
      int file, pipefd[2];                  // prepare to pipe
      pipe(pipefd);                         // pipe time
      dup2(pipefd[READ_END], STDIN_FILENO); // let there be pipe
      close(pipefd[READ_END]);              // close the pipe

      while (NULL != curr)
      {
        if (0 > (file = open(curr->filename, O_RDONLY))) // try to read the file
        {
          perror("error reading file"); // oops
          exit(1);                      // byebye
        }
        _pipe_data(pipefd[WRITE_END], file); // pipe it
        close(file);                         // we done
        curr = curr->next_node;              // proceed
      }

      close(pipefd[WRITE_END]);

      if (NULL != command->rdout)
      {
        pipe(pipefd);

        if (fork() == 0)
        { // file writer child
          _stdout_redirection_worker(command->rdout, pipefd[READ_END]);
        }

        dup2(pipefd[WRITE_END], STDOUT_FILENO); // Redirect STDOUT to the input of the pipe
        close(pipefd[WRITE_END]);               // close the old file handle
      }

      execve(execargs[0], execargs, envp); // if execution succeeds, child process stops here
      printf("couldn't execute: %s\n", command->command);
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

    return 1;
  }
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

  char buffer[MAXLINE]; // temporary buffer for fgets

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

    shell_command *commands[MAXARGS]; // an array of commands
    int exec_ok = 0 < parse_commands(buffer, commands, MAXARGS);

    // we still need to iterate all the commands to free them even if they aren't executable since they're malloc'dd
    for (shell_command **command = &commands[0]; *command != NULL; ++command)
    {
      if (exec_ok)
        process_command(*command, envp);
      free_command(*command);
    }
  }

  shutdown();

  return 0;
}

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
#include "bgjob.h"

char *prompt_prefix = NULL;
int b_noclobber = 0;
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
  shutdown_job_manager();
  free(prompt_prefix);
}

void cleanup_redirection(shell_command *command, int oldstdout, int oldstderr, int oldstdin)
{
  // Put the old descriptors back

  if (oldstdout != -1)
    dup2(oldstdout, STDOUT_FILENO);
  if (oldstderr != -1)
    dup2(oldstderr, STDERR_FILENO);
  if (oldstdin != -1)
    dup2(oldstdin, STDIN_FILENO);

  // The first node of each redirection list may be an already open pipe for IPC
  // If not closed by the parent the child will hang infinitely

  if (command->rdin && command->rdin->fd >= 0) // close the pipe or things get screwed up
    close(command->rdin->fd);

  if (command->rdout && command->rdout->fd >= 0) // close the pipe or things get screwed up
    close(command->rdout->fd);
}

int process_command(shell_command *command, char **envp)
{
  char *arguments[MAXARGS];  // an array of tokens
  pid_t pid, writerpid = -1; // pid of the executed command
  int status, writerstatus;  // status of the executed command
  // parse command line into tokens (stored in buffer)
  str_split_n(command->command, " ", arguments, MAXARGS);

  // Our first argument should be the program/command. If null, we have nothing to execute
  if (arguments[0] == NULL)
    return 0;

  int pipefd[2], pipefderr[2], inpipefd[2];
  int oldstdout = -1, oldstderr = -1, oldstdin = -1;

  if (NULL != command->rdout)
  {
    pipe(pipefd);    // create a pipe
    pipe(pipefderr); // create an error pipe

    if ((writerpid = fork()) == 0) // file writer child
    {
      close(pipefd[WRITE_END]);                                                                          // Child worker will not use the write end
      close(pipefderr[WRITE_END]);                                                                       // Child worker will not use the write end
      redirect_output_worker(command->rdout, pipefd[READ_END], pipefderr[READ_END], dup(STDERR_FILENO)); // continuously write stdout to the files
      exit(0);                                                                                           // exit when we're done
    }

    // parent must set up the pipe

    close(pipefd[READ_END]);                   // parent process will not use the read end
    close(pipefderr[READ_END]);                // parent process will not use the read end
    oldstdout = dup(STDOUT_FILENO);            // dup old STDOUT
    oldstderr = dup(STDERR_FILENO);            // dup old STDERR
    dup2(pipefd[WRITE_END], STDOUT_FILENO);    // Redirect STDOUT to the input of the pipe
    dup2(pipefderr[WRITE_END], STDERR_FILENO); // Redirect STDERR to the input of the pipe
    close(pipefd[WRITE_END]);                  // close the old file handle
    close(pipefderr[WRITE_END]);               // close the old file handle
  }

  if (NULL != command->rdin) // input redirection
  {
    pipe(inpipefd);                                          // create a pipe
    oldstdin = dup(STDIN_FILENO);                            // dup old STDIN
    dup2(inpipefd[READ_END], STDIN_FILENO);                  // replace stdin with the read end
    close(inpipefd[READ_END]);                               // close the old reference
    if (!redirect_input(command->rdin, inpipefd[WRITE_END])) // feed the pipe
      exit(1);                                               // error go brr
    close(inpipefd[WRITE_END]);                              // close the pipe
  }

  if (try_exec_builtin(arguments))
  {
    cleanup_redirection(command, oldstdout, oldstderr, oldstdin);
    if (writerpid != -1)                    // only wait if the filewriter actually ran
      waitpid(writerpid, &writerstatus, 0); // we don't really care if this works or not but if possible we want to wait for the file worker to exit
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
      printf("couldn't execute: %s\n", command->command);
      exit(127);
    }

    // parent

    cleanup_redirection(command, oldstdout, oldstderr, oldstdin);

    if ((pid = waitpid(pid, &status, 0)) < 0)
      printf("waitpid error\n");

    if (writerpid != -1)                    // only wait if the filewriter actually ran
      waitpid(writerpid, &writerstatus, 0); // we don't really care if this works or not but if possible we want to wait for the file worker to exit

    if (WIFEXITED(status)) // S&R p. 239
    {
      int exitcode = WEXITSTATUS(status);
      if (exitcode != 0)
        printf("process terminated with code %d\n", exitcode);
    }
  }

  return 1;
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
  initialize_job_manager();

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

    shell_command *first_cmd = parse_commands(buffer);

    // we still need to iterate all the commands to free them even if they aren't executable since they're malloc'dd
    for (shell_command *command = first_cmd; command != NULL; command = command->next_node)
    {
      if (command->b_background)
      {
        if (command->next_node)
        {
          fprintf(stderr, "cannot pipe output from a background job\n");
          break;
        }

        int fpid = fork();
        if (fpid < 0)
        {
          fprintf(stderr, "failed to create background job\n");
          break;
        }
        else if (fpid == 0)
        {
          process_command(command, envp);
          exit(0);
        }
        else
        {
          register_process(fpid);
        }
      }
      else
        process_command(command, envp);
    }

    free_commands(first_cmd);
  }

  shutdown();

  return 0;
}

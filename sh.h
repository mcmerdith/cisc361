#ifndef SHHEADER
#define SHHEADER

#include "get_path.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <glob.h>
#include <sys/wait.h>

int processBuiltins(char *arguments[]);
void list(char *dir);
void printenv(char **envp);

#define PROMPTMAX 64
#define MAXARGS 16
#define MAXLINE 128
#endif
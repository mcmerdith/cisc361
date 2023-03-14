#pragma once

#include "get_path.h"

char *exec_which(char *arg);
void exec_where(char *arg);
char *path_search(char *command, struct pathelement *p, int bPrintOrReturn);
void free_path(struct pathelement *p);
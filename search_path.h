#pragma once

#include "get_path.h"

char *exec_which(char *arg);
char *exec_where(char *arg);
char *which(char *command, struct pathelement *p, int bIsWhere);
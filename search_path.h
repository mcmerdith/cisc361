#pragma once

#include "get_path.h"

char *execWhich(char *arg);
char *execWhere(char *arg);
char *which(char *command, struct pathelement *p, int bIsWhere);
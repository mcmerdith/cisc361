#pragma once

#include "sh.h"

// expand wildcards in arguments into out_execargs.
// returns a pointer to a NULL terminated array with at most MAX_ARGS elements including the NULL-terminator
int expand_n_wildcards(char *arguments[], char *execargs[], int max_args);

// expand BUFFER into OUT_SEGMENTS.
// OUT_SEGMENTS will be a NULL terminated array with at most MAX_ARGS elements including the NULL-terminator
void str_split_n(char *buffer, char *delimiter, char *out_segments[], int max_args);

// join elements of ARRAY with DELIMITER
// ARRAY should be a NULL terminated array, returns a pointer to a malloc'd string
char *join_array(char *array[], char *delimiter);

// removes whitespace from INPUT in place
// Note: memory MUST be writable or a segfault will occur
void trim_whitespace(char *input);

// get the length of a NULL-terminated array
int array_len(void *array[], int max_len);

// process a character buffer into a list of shell commands with associated file
// descriptors for piping, redirecting, etc.
// caller is responsible for freeing all OUT_COMMANDS
int parse_commands(char *buffer, shell_command *out_commands[], int max_commands);

// free's all related resources for COMMAND
void free_command(shell_command *command);
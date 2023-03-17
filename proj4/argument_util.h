#pragma once

// expand wildcards in arguments into out_execargs.
// returns a pointer to a NULL terminated array with at most MAX_ARGS elements including the NULL-terminator
int expand_n_wildcards(char *arguments[], char *execargs[], int max_args);

// expand BUFFER into ARGUMENTS.
// ARGUMENTS will be a NULL terminated array with at most MAX_ARGS elements including the NULL-terminator
void str_split_n(char *buffer, char *delimiter, char *arguments[], int max_args);

// join elements of ARRAY with DELIMITER
// ARRAY should be a NULL terminated array, returns a pointer to a malloc'd string
char *join_array(char *array[], char *delimiter);
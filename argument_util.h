#pragma once

// expand wildcards in arguments into out_execargs.
// returns a pointer to a NULL terminated array with at most MAX_ARGS elements including the NULL-terminator
int expand_n_wildcards(char *arguments[], char *execargs[], int max_args);

// expand BUFFER into ARGUMENTS.
// ARGUMENTS will be a NULL terminated array with at most MAX_ARGS elements including the NULL-terminator
void expand_n_arguments(char *buffer, char *arguments[], int max_args);
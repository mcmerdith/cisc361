#pragma once

// expand wildcards in ARGUMENTS up to at most MAX_ARGS characters in OUT_EXECARGS
// *OUT_EXECARGS must be at least MAX_ARGS + 1 char* allocated to accomodate the terminating NULLPTR
int process_n_externals(char *arguments[], char ***out_execargs, int max_args);

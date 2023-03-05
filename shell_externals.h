#pragma once

// expand wildcards in ARGUMENTS up to at most MAX_ARGS characters in OUT_EXECARGS
// *OUT_EXECARGS must be at least MAX_ARGS + 1 char* allocated to accomodate the terminating NULLPTR
int expand_wildcards(char *arguments[], char *execargs[], int max_args);

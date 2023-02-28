#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
typedef struct mp3
{
  char       *artist;
  char       *title;
  int         runtime;
  struct mp3 *next;
  struct mp3 *prev;
} mp3_t; 

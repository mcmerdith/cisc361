#pragma once

/*
  get_path.h
  Ben Miller

*/

struct pathelement
{
  char *element;            /* a dir in the path */
  struct pathelement *next; /* pointer to next node */
};

/* function prototype.
   It returns a pointer to a linked list for the path elements. */
struct pathelement *get_path();

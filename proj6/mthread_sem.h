#include "mthread_lib.h"

typedef struct sem_t
{
    int count;
    tcb *queue;
} sem_t;
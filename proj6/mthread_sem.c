#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mthread_sem.h"

int sem_init(sem_t **sp, int sem_count)
{
    *sp = malloc(sizeof(sem_t));
    (*sp)->count = sem_count;
    (*sp)->queue = NULL;
}

void sem_wait(sem_t *sp)
{
    --sp->count;
}

void sem_signal(sem_t *sp)
{
    ++sp->count;
}

void sem_destroy(sem_t **sp)
{
}
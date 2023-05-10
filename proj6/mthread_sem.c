#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mthread_lib.h"
#include "mthread_sem.h"
#include "ud_thread.h"

extern tcb *running_queue; // Queue for running threads
extern tcb *ready_queue;   // Queue for ready threads

void _sem_enqueue(sem_t *sp, tcb *_tcb)
{
    tcb **queue = &sp->queue;

    while (*queue != NULL)
        queue = &(*queue)->next;

    *queue = _tcb;
}

tcb *_sem_dequeue(sem_t *sp)
{
    tcb *queue = sp->queue;

    if (queue)
    {
        sp->queue = queue->next;
        queue->next = NULL;
    }

    return queue;
}

void sem_init(sem_t **sp, int sem_count)
{
    *sp = malloc(sizeof(sem_t));
    (*sp)->count = sem_count;
    (*sp)->queue = NULL;
}

void sem_wait(sem_t *sp)
{
    --sp->count;

    if (sp->count >= 0) // if the semaphore is ready the caller is good to go
        return;

    // otherwise they'll have to wait

    _sem_enqueue(sp, running_queue); // add the running thread to the waiting queue

    _run_next_task(0, 0);
}

void sem_signal(sem_t *sp)
{
    ++sp->count;

    // if (sp->count < 0)
    //     return;

    tcb *queued = _sem_dequeue(sp);

    if (queued)
        _add_ready_thread(queued);
}

void sem_destroy(sem_t **sp)
{
    // Add all waiting threads to the ready queue
    tcb *queued = (*sp)->queue;
    while (queued != NULL)
    {
        printf("A thread was waiting when a semaphore was destroyed, this likely indicates a semantic problem with your application\n");
        _add_ready_thread(queued);
        queued = queued->next;
    }

    free(*sp);
}
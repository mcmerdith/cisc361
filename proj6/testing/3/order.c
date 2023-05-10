#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ud_thread.h"

sem_t *first, *last, *complete;

void worker1(int t_id)
{
    sem_wait(first);
    printf("I am worker 1\n");
    sem_signal(last);

    sem_signal(complete);

    t_terminate();
}

void worker2(int t_id)
{
    printf("I am worker 2\n");
    sem_signal(first);
    sem_signal(first);

    sem_signal(complete);

    t_terminate();
}

void worker3(int t_id)
{
    sem_wait(last);
    sem_wait(last);
    printf("I am worker 3\n");

    sem_signal(complete);

    t_terminate();
}

void worker4(int t_id)
{
    sem_wait(first);
    printf("I am worker 4\n");
    sem_signal(last);

    sem_signal(complete);

    t_terminate();
}

int main(int argc, char *argv[])
{
    t_init();

    sem_init(&first, 0);
    sem_init(&last, 0);
    sem_init(&complete, 0);

    t_create(worker3, 3, 0);
    t_create(worker4, 4, 0);
    t_create(worker2, 2, 0);
    t_create(worker1, 1, 0);

    sem_wait(complete);
    sem_wait(complete);
    sem_wait(complete);
    sem_wait(complete);

    sem_destroy(&first);
    sem_destroy(&last);
    sem_destroy(&complete);

    printf("done.......\n");

    t_shutdown();
    return 0;
}
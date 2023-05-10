#include <stdio.h>
#include <unistd.h>
#include "ud_thread.h"

// If done correctly, each child should print their "before" message
// before either prints their "after" message. Test by adding sleep(1)
// calls in various locations.

sem_t *s1, *s2, *complete;

void child_1(int t_id)
{
    printf("child 1: before\n");
    // what goes here?
    sleep(2);
    sem_signal(s1);
    sem_wait(s2);
    printf("child 1: after\n");

    sem_signal(complete);

    t_terminate();
}

void child_2(int t_id)
{
    printf("child 2: before\n");
    // what goes here?
    sem_signal(s2);
    sem_wait(s1);
    printf("child 2: after\n");

    sem_signal(complete);

    t_terminate();
}

int main(int argc, char *argv[])
{
    t_init();

    printf("parent: begin\n");
    // init semaphores here
    sem_init(&s1, 0);
    sem_init(&s2, 0);
    sem_init(&complete, -1);
    t_create(child_1, 0, 0);
    t_create(child_2, 1, 0);

    // this thread library doesn't have the equivalent for "join" so this will do
    sem_wait(complete);

    sem_destroy(&s1);
    sem_destroy(&s2);
    sem_destroy(&complete);
    printf("parent: end\n");

    t_shutdown();

    return 0;
}
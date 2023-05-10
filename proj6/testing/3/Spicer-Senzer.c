/*
 * Spicer_Senzer_Phase_3_Test
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ud_thread.h"

sem_t *s;
int resource = 0;

void read_function(int val)
{
    int i;
    int res_before, res_after;

    for (i = 0; i < 1000; i++)
    {
        printf("I am READ thread %d (%d)\n", val, i);
        fflush(stdout);

        sem_wait(s);

        res_before = resource;
        printf("  [%d READ %d] resource = %d\n", val, i, resource);
        fflush(stdout);
        t_yield();
        printf("  ** [%d READ %d] returns from t_yield()\n", val, i);
        fflush(stdout);
        res_after = resource;

        if (res_before != res_after)
        {

            fprintf(stderr, "[THREAD %d] error in the semaphore program \n", val);
            fflush(stdout);
            exit(-1);
        }

        sem_signal(s);
    }

    t_terminate();
}

void write_function(int val)
{
    int i;

    for (i = 0; i < 1000; i++)
    {
        printf("I am WRITE thread %d (%d)\n", val, i);
        fflush(stdout);
        sem_wait(s);

        resource = rand() % 100;
        printf("  [%d WRITE %d] resource = %d\n", val, i, resource);
        fflush(stdout);
        sem_signal(s);

        t_yield();
    }

    t_terminate();
}

int main(void)
{

    int i;

    t_init();
    sem_init(&s, 1);

    t_create(write_function, 1, (rand() % 2));
    t_create(read_function, 97, (rand() % 2));
    t_create(write_function, 2, (rand() % 2));
    t_create(read_function, 98, (rand() % 2));
    t_create(write_function, 3, (rand() % 2));
    t_create(read_function, 99, (rand() % 2));
    t_create(write_function, 4, (rand() % 2));
    t_create(read_function, 100, (rand() % 2));

    for (i = 0; i < 60; i++)
    {
        printf("I am main thread (%d)...\n", i);
        fflush(stdout);
        sleep(1);
        t_yield();
    }

    sem_destroy(&s);
    printf("Beginning shutdown...\n");
    fflush(stdout);
    t_shutdown();
    printf("Down with shutdown...\n");
    fflush(stdout);

    return 0;
}
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "bgjob.h"

job *job_head = NULL;
pthread_mutex_t job_mutex;

void _add_job(job *new_job)
{
    pthread_mutex_lock(&job_mutex);
    job **current = &job_head;
    while (1)
    {
        if (*current != NULL && (*current)->next_node == NULL) // try and find an element with a next_node
        {
            (*current)->next_node = new_job;
            new_job->prev_node = *current;
            break;
        }
        else if (*current == NULL)
        {
            *current = new_job;
            break;
        }

        current = &(*current)->next_node;
    }
    pthread_mutex_unlock(&job_mutex);
}

void *_thread_job_manager(void *arg)
{
    job **curr, *temp;
    int status, i;

    while (1)
    {
        pthread_mutex_lock(&job_mutex);

        curr = &job_head;

        i = 0;

        while (*curr != NULL)
        {
            ++i; // increment the job id

            if (waitpid((*curr)->pid, &status, WNOHANG) == 0)
            {
                curr = &(*curr)->next_node;
                continue;
            }

            printf("[%d] background job (%d) terminated with code %d\n", i, (*curr)->pid, WEXITSTATUS(status));

            temp = *curr;

            if (temp->prev_node)
                temp->prev_node->next_node = temp->next_node; // remove this element from the list

            if (temp->next_node)
                temp->next_node->prev_node = temp->prev_node; // remove this element from the list

            *curr = temp->next_node; // move the pointer

            if (temp->opt_descriptor)       // if we have a descriptor
                free(temp->opt_descriptor); // free the descriptor
            free(temp);                     // free the job
        }

        pthread_mutex_unlock(&job_mutex);

        sleep(1);
    }
}

pthread_t manager_id;
int b_watchuser_running = 0;
pthread_t watchuser_id;

void initialize_job_manager()
{
    run_thread(&manager_id, &_thread_job_manager);
}

void shutdown_job_manager()
{
    pthread_cancel(manager_id);     // stop the thread
    pthread_join(manager_id, NULL); // wait for exit

    if (b_watchuser_running) // only stop the watchuser thread if its running
    {
        pthread_cancel(watchuser_id);     // stop the thread
        pthread_join(watchuser_id, NULL); // wait for exit
    }
}

void run_thread(pthread_t *thread_id, void *(*thread_method)(void *))
{
    pthread_create(thread_id, NULL, thread_method, NULL);
}

void register_process(int pid, char *opt_descriptor)
{
    job *new_job = malloc(sizeof(job));
    new_job->pid = pid;

    // Jobs may optionally have a string description
    if (opt_descriptor)
    {
        new_job->opt_descriptor = calloc(strlen(opt_descriptor) + 1, sizeof(char));
        strcpy(new_job->opt_descriptor, opt_descriptor);
    }
    else
    {
        new_job->opt_descriptor = NULL;
    }

    new_job->prev_node = new_job->next_node = NULL;

    _add_job(new_job);
}
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
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
    int status;

    while (1)
    {
        pthread_mutex_lock(&job_mutex);

        curr = &job_head;

        while (*curr != NULL)
        {
            if (waitpid((*curr)->pid, &status, WNOHANG) == 0)
            {
                curr = &(*curr)->next_node;
                continue;
            }

            printf("background job [%d] terminated with code %d\n", (*curr)->pid, WEXITSTATUS(status));

            temp = *curr;

            if (temp->prev_node)
                temp->prev_node->next_node = temp->next_node; // remove this element from the list

            if (temp->next_node)
                temp->next_node->prev_node = temp->prev_node;

            *curr = temp->next_node;

            free(temp);
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

void register_process(int pid)
{
    job *new_job = malloc(sizeof(job));
    new_job->pid = pid;
    new_job->prev_node = new_job->next_node = NULL;

    _add_job(new_job);
}
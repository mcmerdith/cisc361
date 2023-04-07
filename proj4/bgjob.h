#pragma once

#include <pthread.h>

typedef struct job
{
    int pid;                           // the pid of the process, if applicable
    struct job *next_node, *prev_node; // linked-list stuff
} job;

// initialize
void initialize_job_manager();

// shutdown
void shutdown_job_manager();

// Run THREAD_METHOD as a new thread
void run_thread(pthread_t *thread_id, void *(*thread_method)(void *));

// Register a process to be watched
void register_process(int pid);
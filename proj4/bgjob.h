#pragma once

#include <pthread.h>

typedef struct job
{
    int pid;                           // the pid of the process, if applicable
    char *opt_descriptor;              // an optional descriptor for the process
    struct job *next_node, *prev_node; // linked-list stuff
} job;

// initialize
void initialize_job_manager();

// shutdown
void shutdown_job_manager();

// Run THREAD_METHOD as a new thread
void run_thread(pthread_t *thread_id, void *(*thread_method)(void *));

// print the current managed jobs to stdout
void print_jobs();

// Release a process from the job manager.
// Caller must waitpid(2) the returned process or it will become a zombie
// Returns the PID of the process associated with the job or -1 if there is no job with the requested id
int release_process(int job_id);

// Register a process to be watched
void register_process(int pid, char *opt_descriptor);
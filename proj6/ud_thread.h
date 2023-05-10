/*
 * thread library function prototypes
 */

// Create a new thread from FUNCTION with id THREAD_ID and priority PRIORITY
void t_create(void (*function)(int), int thread_id, int priority);

// Terminate a thread. Semantically, this should be called at the end of every thread function
void t_terminate(void);

// Voluntarily release CPU control and be placed at the end of the thread queue
void t_yield(void);

// Initialize the thread library
void t_init(void);

// Shutdown and cleanup the thread library
void t_shutdown(void);

/**
 * semaphore
 */

// Create a new semaphore pointed to by sp with a count value of sem_count.
int sem_init(sem_t **sp, int sem_count);

// Current thread does a wait (P) on the specified semaphore.
void sem_wait(sem_t *sp);

// Current thread does a signal (V) on the specified semaphore.
// Follow the Mesa semantics (p. 9 of OSTEP Chapter 30 Condition Variables) where the thread that signals continues,
// and the first waiting (blocked) thread (if there is any) becomes ready.
void sem_signal(sem_t *sp);

// Free any memory related to the specified semaphore.
// If there are queued TCBs, they are moved to the ready queue, and this may imply that the semantics of the application might be incorrect.
void sem_destroy(sem_t **sp);

typedef void sem_t;
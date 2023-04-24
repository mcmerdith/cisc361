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

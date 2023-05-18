#include "mthread_sem.h"
#include "mthread_message.h"

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
void sem_init(sem_t **sp, int sem_count);

// Current thread does a wait (P) on the specified semaphore.
void sem_wait(sem_t *sp);

// Current thread does a signal (V) on the specified semaphore.
// Follow the Mesa semantics (p. 9 of OSTEP Chapter 30 Condition Variables) where the thread that signals continues,
// and the first waiting (blocked) thread (if there is any) becomes ready.
void sem_signal(sem_t *sp);

// Free any memory related to the specified semaphore.
// If there are queued TCBs, they are moved to the ready queue, and this may imply that the semantics of the application might be incorrect.
void sem_destroy(sem_t **sp);

// Create a mailbox pointed to by MB
void mbox_create(mbox **mb);

// Destroy any state related to the mailbox pointed to by MB
void mbox_destroy(mbox **mb);

// Deposit message MESSAGE of length LEN into the mailbox pointed to by MB
void mbox_deposit(mbox *mb, char *message, int len);

// Withdraw the first message from the mailbox pointed to by MB into MESSAGE and set the messages length in LEN
// The caller is responsible for allocating a buffer for MESSAGE
// If there is no message, LEN will be set to 0. Caller will NOT be blocked
// Only one message is returned per call and are retrieved in the order deposited
void mbox_withdraw(mbox *mb, char *message, int *len);

// Send a message to the thread whose tid is TID. MESSAGE is the message and LEN specifies the length
void send(int tid, char *message, int len);

// Wait for a receive a message from another thread. Specify the thread to receive from in TID or provide 0 to receive from any thread
// If no messages are ready, the caller will be blocked until a message is available
// The message will be stored in MESSAGE and the length in LEN
// The caller is responsible for allocating a buffer for MESSAGE
// Only one message is returned per call and are retrieved in the order deposited
void receive(int *tid, char *message, int *len);
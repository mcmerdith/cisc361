#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ucontext.h>
#include <sys/mman.h>
#include "mthread_lib.h"

#include <valgrind/valgrind.h>

/**
 * Memory Management
 */

// Create a new TCB with thread id ID and thread priority PRIORITY
// TCB context is initially empty, with no next element
tcb *_new_tcb(int id, int priority)
{
  tcb *newtcb = malloc(sizeof(tcb));

  newtcb->id = id;
  newtcb->priority = priority;
  newtcb->context = malloc(sizeof(ucontext_t));
  newtcb->valgrind_stackid = 0;
  newtcb->next = NULL;

  return newtcb;
}

// Free all resources associated with a TCB
void _free_tcb(tcb *_tcb)
{
  free(_tcb->context);
  VALGRIND_STACK_DEREGISTER(_tcb->valgrind_stackid);
  free(_tcb);
}

/**
 * TCB Context Wrappers
 */

// Set the TARGET tcb context to the current running context
void _save_thread_context(tcb *target)
{
  getcontext(target->context);
}

// Set the current running context to the TARGET tcb context
void _load_thread_context(tcb *target)
{
  setcontext(target->context);
}

/**
 * Thread Queues
 */

tcb *running_queue = NULL; // Queue for running threads
tcb *ready_queue = NULL;   // Queue for ready threads

// Add a TCB to the running queue
// Note: this does not actually run the thread
void _add_running_thread(tcb *thread)
{
  tcb **target = &running_queue; // start at the head

  while (*target != NULL)      // while we have an element
    target = &(*target)->next; // move on

  *target = thread;
}

// Add a TCB to the ready queue
// Note: this does not actually stop running the thread
void _add_ready_thread(tcb *thread)
{
  tcb **target = &ready_queue; // start at the head

  while (*target != NULL)      // while we have an element
    target = &(*target)->next; // move on

  *target = thread; // place the new element in the last empty spot
}

// Pop a thread off the running queue
// If B_REMOVE, all resources for the TCB will be freed,
// otherwise the thread will be moved back to the ready queue
// Note: this does not actually stop running the thread
void _pop_running_queue(int b_remove)
{
  if (running_queue == NULL)
    return;

  tcb *popped = running_queue;         // Get the reference to be popped
  running_queue = running_queue->next; // Shift the LL-pointer
  popped->next = NULL;                 // Seperate the popped reference from the LL

  if (b_remove)
    _free_tcb(popped);
  else
    _add_ready_thread(popped);
}

// Pop a thread off the ready queue onto the end of the running queue
// Note: this does not actually run the thread
void _pop_ready_to_running()
{
  if (ready_queue == NULL)
    return;

  tcb *popped = ready_queue;       // Get the reference to be popped
  ready_queue = ready_queue->next; // Shift the LL-pointer
  popped->next = NULL;             // Seperate the popped reference from the LL

  _add_running_thread(popped);
}

// Run the next task in the ready queue
// If B_END_CURRENT, the resources associated with the TCB from the running queue will be freed,
// otherwise, the currently running thread will be moved to the back of the ready queue
void _run_next_task(int b_end_current)
{
  if (ready_queue == NULL) // no next task, let the current one keep running
    return;

  int no_swap = running_queue == NULL; // Conditional needs to be checked before popping

  _pop_ready_to_running(); // Move a ready thread into the running queue

  if (no_swap)
  {                                      // If no thread is currently running we can't save the current context
    _load_thread_context(running_queue); // So just load the new context instead
  }
  else
  {
    tcb *running = running_queue;      // save a reference to the currently running thread
    _pop_running_queue(b_end_current); // pop it off the queue

    if (b_end_current)
    {                                      // if we are done with the current thread
      _load_thread_context(running_queue); // just continue to the next one
    }
    else
    {                                                        // otherwise
      swapcontext(running->context, running_queue->context); // save the old and load the new
    }
  }
}

// When a signal is received, run the next ready task
void _sig_handler(int sig)
{
  _run_next_task(0);
}

void t_yield()
{
  HOLD();

  _run_next_task(0);

  RELEASE();
}

void t_init()
{
  SIGSETUP(_sig_handler);

  HOLD();

  tcb *main_tcb = _new_tcb(0, 1); // main thread is id 0, priority 1
  _save_thread_context(main_tcb); // save the current context as the main thread
  _add_running_thread(main_tcb);  // add it as the currently running thread

  // ualarm(10000, 10000); phase 2, we'll get back to this

  RELEASE();
}

void t_create(void (*function)(int), int id, int priority)
{
  HOLD();

  tcb *newtcb = _new_tcb(id, priority); // create a TCB
  _save_thread_context(newtcb);         // save the current context

  // Make a stack for the thread
  size_t sz = 0x10000;
  ucontext_t *uc = newtcb->context;
  uc->uc_stack.ss_sp = mmap(0, sz,
                            PROT_READ | PROT_WRITE | PROT_EXEC,
                            MAP_PRIVATE | MAP_ANON, -1, 0);
  // uc->uc_stack.ss_sp = malloc(sz); /* new statement */
  uc->uc_stack.ss_size = sz;
  uc->uc_stack.ss_flags = 0;
  newtcb->valgrind_stackid = VALGRIND_STACK_REGISTER(uc->uc_stack.ss_sp, uc->uc_stack.ss_sp + sz);

  makecontext(uc, (void (*)(void))function, 1, id); // Set the context to be the thread function
  _add_ready_thread(newtcb);                        // Add it to the ready queue

  RELEASE();
}

void t_terminate()
{
  HOLD();

  _run_next_task(1); // Run the next task, ending the current one

  RELEASE();
}

void t_shutdown()
{
  HOLD();

  tcb *current = ready_queue, // Start at the beginning of the ready queue
      *temp;

  while (current != NULL) // Free all the ready TCBs
  {
    temp = current;
    current = current->next;
    _free_tcb(temp);
  }

  current = running_queue; // Start at the beginning of the running queue

  while (current != NULL) // Free all the TCBs
  {
    temp = current;
    current = current->next;
    _free_tcb(temp);
  }

  ready_queue = running_queue = NULL; // Reset the pointers to avoid segfaulting

  RELEASE();
}

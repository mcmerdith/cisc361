#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ucontext.h>
#include <sys/mman.h>
#include "mthread_lib.h"

#include <valgrind/valgrind.h>

tcb *running_queue = NULL;
tcb *ready_queue = NULL;

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

void _free_tcb(tcb *_tcb)
{
  free(_tcb->context);
  VALGRIND_STACK_DEREGISTER(_tcb->valgrind_stackid);
  free(_tcb);
}

// Set the TARGET tcb context to the current running context
void _save_thread_context(tcb *target)
{
  getcontext(target->context);
}

void _load_thread_context(tcb *target)
{
  setcontext(target->context);
}

void _add_running_thread(tcb *thread)
{
  tcb **target = &running_queue;

  while (*target != NULL)
    target = &(*target)->next;

  *target = thread;
}

void _add_ready_thread(tcb *thread)
{
  tcb **target = &ready_queue;

  while (*target != NULL)
    target = &(*target)->next;

  *target = thread;
}

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

void _pop_ready_to_running()
{
  if (ready_queue == NULL)
    return;

  tcb *popped = ready_queue;       // Get the reference to be popped
  ready_queue = ready_queue->next; // Shift the LL-pointer
  popped->next = NULL;             // Seperate the popped reference from the LL

  _add_running_thread(popped);
}

void _run_next_task(int b_end_current)
{
  if (ready_queue == NULL) // no next task, let the current one keep running
    return;

  int no_swap = running_queue == NULL;

  _pop_ready_to_running();

  if (no_swap)
  {
    _load_thread_context(running_queue);
  }
  else
  {
    tcb *running = running_queue;
    _pop_running_queue(b_end_current);
    if (b_end_current)
    {
      _load_thread_context(running_queue);
    }
    else
    {
      swapcontext(running->context, running_queue->context);
    }
  }
}

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

  tcb *main_tcb = _new_tcb(0, 1);
  _save_thread_context(main_tcb);
  _add_running_thread(main_tcb);

  // ualarm(10000, 10000); phase 2?

  RELEASE();
}

void t_create(void (*function)(int), int id, int priority)
{
  HOLD();

  tcb *newtcb = _new_tcb(id, priority);
  _save_thread_context(newtcb);

  size_t sz = 0x10000;
  ucontext_t *uc = newtcb->context;
  uc->uc_stack.ss_sp = mmap(0, sz,
                            PROT_READ | PROT_WRITE | PROT_EXEC,
                            MAP_PRIVATE | MAP_ANON, -1, 0);
  // uc->uc_stack.ss_sp = malloc(sz); /* new statement */
  uc->uc_stack.ss_size = sz;
  uc->uc_stack.ss_flags = 0;
  newtcb->valgrind_stackid = VALGRIND_STACK_REGISTER(uc->uc_stack.ss_sp, uc->uc_stack.ss_sp + sz);
  // uc->uc_link = running;
  makecontext(uc, (void (*)(void))function, 1, id);
  // ready = uc;
  _add_ready_thread(newtcb);

  RELEASE();
}

void t_terminate()
{
  HOLD();

  _run_next_task(1);

  RELEASE();
}

void t_shutdown()
{
  HOLD();

  tcb *current = ready_queue, *temp;

  while (current != NULL)
  {
    temp = current;
    current = current->next;
    _free_tcb(temp);
  }

  current = running_queue;

  while (current != NULL)
  {
    temp = current;
    current = current->next;
    _free_tcb(temp);
  }

  ready_queue = running_queue = NULL;

  RELEASE();
}

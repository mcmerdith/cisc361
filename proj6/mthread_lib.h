#pragma once

/*
 * types used by thread library
 */
#include <signal.h>

#ifdef ATOMIC
extern sigset_t *interrupt_signals;
#define SIGSETUP(sig_handler)        \
    sigfillset(interrupt_signals);   \
    struct sigaction action;         \
    action.sa_handler = sig_handler; \
    sigemptyset(&action.sa_mask);    \
    action.sa_flags = SA_RESTART;    \
    sigaction(SIGALRM, &action, NULL)
#define HOLD() sigprocmask(SIG_BLOCK, interrupt_signals, NULL)
#define RELEASE() sigprocmask(SIG_UNBLOCK, interrupt_signals, NULL)
#else
#define SIGSETUP(sig_handler)
#define HOLD()
#define RELEASE()
#endif

typedef struct tcb
{
    int id,
        priority;
    ucontext_t *context;
    int valgrind_stackid;
    struct tcb *next;
} tcb;

void _add_running_thread(tcb *thread);
void _add_ready_thread(tcb *thread);
void _pop_running_queue(int b_remove, int b_make_ready);
void _pop_ready_to_running();
void _run_next_task(int b_end_current, int b_make_ready);
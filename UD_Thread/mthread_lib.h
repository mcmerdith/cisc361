/*
 * types used by thread library
 */
#include "ud_thread.h"
#include <signal.h>

sigset_t *interrupt_signals;

#ifdef ATOMIC
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

/*
 * Evan Shone, Jake Hoffman CISC 361 Spring 2016
 * This shows the creation of some threads, yielding in 
 * them, then terminating all of them. Then yielding in 
 * the main thread to ensure robustness. It then rebuilds
 * all of the threads and tests the shutdown method
 */

#include <stdio.h>
#include "mthread.h"

int NUM_THREADS = 5;
int NUM_ITERATIONS = 3;

void my_thread(int thr_id) {

  int yield_i = 0;

  for ( ; yield_i < NUM_ITERATIONS; yield_i++) {
    printf("Thread id:%d\titer:%d\n", thr_id, yield_i+1);
    t_yield();
  }

  printf("Thread id:%d\tterminating\n", thr_id);
  t_terminate();
  printf("Thread id:%d\tERROR: After terminate\n", thr_id);
}

int main(void){

  int thread_i = 1, yield_i = 0;
 
  printf("About to initialize library\n");
  // init thread library
  t_init();
  printf("Library initialized\n");

  
  // create NUM_THREAD threads with id: [1, NUM_THREADS]
  printf("---------------------------------------------------\n");
  printf("About to make %d threads...\n", NUM_THREADS);
  for ( ; thread_i < NUM_THREADS+1; thread_i++) {
    printf("main creating thread id:%d\n", thread_i);
    t_create(my_thread, thread_i, 1);
  }

  // yield NUM_ITERATIONS times 
  printf("---------------------------------------------------\n");
  printf("%d threads created, about to yield...\n", NUM_THREADS);
  for ( ; yield_i < NUM_ITERATIONS; yield_i++) {
    printf("---------------------------------------------------\n");
    printf("Thread id:main\titer:%d\n", yield_i+1);
    t_yield();
  }
  
  // yield once more so each thread terminates
  printf("---------------------------------------------------\n");
  printf("Threads about to terminate...\n");
  t_yield();

  // yield again to check robustness of yield function
  // there are no other threads at this point so yield
  // should keep this thread running
  printf("---------------------------------------------------\n");
  printf("Call yield() in main (no other threads to run)\n");
  t_yield();
  printf("After yield() in main\n");

  // build up some more threads
  printf("---------------------------------------------------\n");
  printf("About to make %d threads...\n", NUM_THREADS);
  for ( ; thread_i < (NUM_THREADS*2)+1; thread_i++) {
    printf("main creating thread id:%d\n", thread_i);
    t_create(my_thread, thread_i, 1);
  }
  
  // test t_shutdown
  printf("---------------------------------------------------\n");
  printf("About to invoke t_shutdown()\n");
  t_shutdown();
  printf("After t_shutdown()\n");
  
  // yield again to check robustness of yield function
  // there are no other threads at this point so yield
  // should keep this thread running
  printf("---------------------------------------------------\n");
  printf("Call yield() in main (no other threads to run)\n");
  t_yield();
  printf("After yield() in main\n");

  // end of the test
  printf("---------------------------------------------------\n");
  printf("TEST COMPLETE\n");

  return 0;
}

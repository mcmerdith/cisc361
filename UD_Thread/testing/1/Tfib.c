/*
 * Multithreaded Fibonacci Generator
 * By: Kyle Sullivan
 * University of Delaware - CISC 361
 *
 * This test case demonstrates how to generate fibonacci numbers using
 * multithreading and dynamic programming
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "mthread.h"

// this lock is used to ensure mutual exclusion
// when accessing the fibonacci array across
// multiple threads
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

// global array to store fibonacci results
long long fib_array[10000];
// note NUM_THREADS and N have to be divisible
int NUM_THREADS = 4;
int N = 92; // max for 64 bits

/*
 * fib
 * - fib(n) returns the nth number of the fibonacci sequence
 *   (note the answer is found using dynamic programming,
 *    this means that previous answers are stored in an array
 *    to speed up the calculation time of the nth + 1 fib number)
 */
long long fib(int n)
{

   // first cover the base cases, no lock
   // needed since the first two elements are given
   if (n == 0)
      return fib_array[0];

   if (n == 1)
      return fib_array[1];

   // retrieve the n - 2 fibonacci number,
   // if it is 0 (aka not previously calculated)
   // then lets calculate it
   pthread_rwlock_rdlock(&rwlock);
   long long tmp1 = fib_array[n - 2];
   pthread_rwlock_unlock(&rwlock);
   if (tmp1 == 0)
   {
      fib(n - 2);
   }

   // retrieve the n - 1 fibonacci number,
   // if it is 0 (aka not previously calculated)
   // then lets calculate it
   pthread_rwlock_rdlock(&rwlock);
   long long tmp2 = fib_array[n - 2];
   pthread_rwlock_unlock(&rwlock);
   if (tmp2 == 0)
   {
      fib(n - 1);
   }

   // we have the n - 2 and n - 1 fibonacci number,
   // now we can calculate the nth fibonacci number
   // when we finish calculating it, store it in the array
   // so we don't don't have to calculate it again later
   long long rval;
   pthread_rwlock_wrlock(&rwlock);
   fib_array[n] = fib_array[n - 1] + fib_array[n - 2];
   rval = fib_array[n];
   pthread_rwlock_unlock(&rwlock);

   // return the nth fiboacci number
   return rval;
}

// generates a thread which calculates fiboacci numbers
void fib_thread(int thr_id)
{

   int n;
   long long fibnum;
   // int n = thr_id;

   // loops through and calculates the fiboacci numbers
   // for the respective thread based on the total number
   // of threads, so if there are 4 threads and this is thread 0,
   // it will find fib(0), fib(4), fib(8) . . . until fib(N)
   for (n = thr_id; n < N; n = n + NUM_THREADS)
   {
      fibnum = fib(n);
      printf("fib[%d] = %lld (thread #%d)\n", n, fibnum, thr_id);
      t_yield();
   }

   printf("Thread %d is done...\n", thr_id);
   t_terminate();
}

/*
 * USAGE:
 * ./multi_fib <N> <num_threads>
 * - generates from 0 to Nth fibonacci number using
 *   the number of threads specified
 *
 * - default, N = 92 (max for 64 bits), and 4 threads
 *
 * NOTE: N and NUM_THREADS need to be divisible
 */
int main(int argc, char *argv[])
{

   // parse the command line for N and NUM_THREADS
   if (argc == 3)
   {
      N = atoi(argv[1]);
      NUM_THREADS = atoi(argv[2]);

      if (N < 0 || N > 92)
      {
         printf("N must be between 0 and 92.\n");
         return 1;
      }

      if (NUM_THREADS < 1)
      {
         printf("NUM_THREADS must be at least 1 thread or more.\n");
         return 1;
      }

      if (N % NUM_THREADS != 0)
      {
         printf("N and NUM_THREADS must be divisible\n");
         return 1;
      }
   }
   printf("Generating the fibonacci sequence from 0 to %d, using %d threads.\n", N, NUM_THREADS);

   // Initialize the beginning of the fibonacci array
   fib_array[0] = 0;
   fib_array[1] = 1;

   // thread library, main thread starts
   t_init();

   // create the threads base on NUM_THREADS
   int i;
   for (i = 0; i < NUM_THREADS; i++)
   {
      t_create(fib_thread, i, 1);
      printf("created fib_thread %d\n", i);
   }

   // go longer than all other threads (to show when t_yield is called with empty ready queue)
   for (i = 0; i < N + 1; i++)
   {

      t_yield();
   }

   // all done
   printf("Begin shutdown...\n");
   t_shutdown();
   printf("Done with shutdown...\n");

   return 0;
}

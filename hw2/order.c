#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t first, last;

void *worker1(void *arg)
{
  sem_wait(&first);
  printf("I am worker 1\n");
  sem_post(&last);
  pthread_exit(NULL);
}

void *worker2(void *arg)
{
  printf("I am worker 2\n");
  sem_post(&first);
  sem_post(&first);
  pthread_exit(NULL);
}

void *worker3(void *arg)
{
  sem_wait(&last);
  sem_wait(&last);
  printf("I am worker 3\n");
  pthread_exit(NULL);
}

void *worker4(void *arg)
{
  sem_wait(&first);
  printf("I am worker 4\n");
  sem_post(&last);
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  pthread_t p1, p2, p3, p4;

  sem_init(&first, 0, 0);
  sem_init(&last, 0, 0);

  pthread_create(&p3, NULL, worker3, NULL);
  pthread_create(&p4, NULL, worker4, NULL);
  pthread_create(&p2, NULL, worker2, NULL);
  pthread_create(&p1, NULL, worker1, NULL);
  pthread_join(p1, NULL);
  pthread_join(p2, NULL);
  pthread_join(p3, NULL);
  pthread_join(p4, NULL);
  printf("done.......\n");
  return 0;
}

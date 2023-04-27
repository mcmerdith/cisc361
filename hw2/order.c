#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t t1, t2, t3;

void *worker1(void *arg)
{
  printf("I am worker 1\n");
  sleep(1);
  sem_post(&t1);
  pthread_exit(NULL);
}

void *worker2(void *arg)
{
  sem_wait(&t1);
  printf("I am worker 2\n");
  sleep(1);
  sem_post(&t2);
  pthread_exit(NULL);
}

void *worker3(void *arg)
{
  sem_wait(&t2);
  printf("I am worker 3\n");
  sleep(1);
  sem_post(&t3);
  pthread_exit(NULL);
}

void *worker4(void *arg)
{
  sem_wait(&t3);
  printf("I am worker 4\n");
  sleep(1);
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  pthread_t p1, p2, p3, p4;

  sem_init(&t1, 0, 0);
  sem_init(&t2, 0, 0);
  sem_init(&t3, 0, 0);

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

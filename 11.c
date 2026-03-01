/*
 * File: 11.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-01
 * Description:
 * NOTE: keep it working & simple
 */

#include <stdio.h>

#include <pthread.h>

#include <unistd.h>

// NOTE: TCNT must be multiple of some barrier
#define TCNT 6

pthread_barrier_t tbarr;

void *routine(void *args) {
  int thread_id = *((int *)args);
  printf("thread %d begun\n", thread_id);

  printf("waiting at barrier\n");
  sleep(1);
  pthread_barrier_wait(&tbarr);
  printf("crossed the barrier\n");
  sleep(1);

  printf("thread %d ended\n", thread_id);
  return NULL;
}

int main() {
  // NOTE: barrier cnt represents the minimum number of steps to be used
  pthread_barrier_init(&tbarr, NULL, 3);
  pthread_t tarr[TCNT];
  int values[TCNT];

  for (int i = 0; i < TCNT; ++i) {
    values[i] = i;
    pthread_create(tarr + i, NULL, &routine, (void *)(&values[i]));
  }

  for (int i = 0; i < TCNT; ++i) {
    pthread_join(tarr[i], NULL);
  }

  pthread_barrier_destroy(&tbarr);
  return 0;
}

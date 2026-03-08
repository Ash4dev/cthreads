/*
 * File: 14.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-07
 * Description: Deadlock scenarios
 * NOTE: keep it working & simple
 */

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define TCNT 2

pthread_mutex_t mx_1, mx_2;
int v1 = 10, v2 = 20;

void *routine(void *args) {
  if (rand() % 2) {
    pthread_mutex_lock(&mx_1);
    sleep(1);
    pthread_mutex_lock(&mx_2);
  } else {
    pthread_mutex_lock(&mx_2);
    sleep(1);
    pthread_mutex_lock(&mx_1);
  }

  // NOTE: deadlock: subset of threads being inactive due to mutual waiting
  // BAD CASE: t1: mx_1, t1: wait for mx_1 : keeps waiting forever
  // recursive mutexes: can be locked multiple times
  // BAD CASE -> t1: mx_1, t2: mx_2, t1: wait for mx_2, t2: wait for mx_1

  // NOTE: mitigation strategies: finite timeout, definite order, acquire once
  // https://arpitbhayani.me/blogs/database-deadlocks/
  pthread_mutex_unlock(&mx_1);
  pthread_mutex_unlock(&mx_2);
  return NULL;
}

int main() {

  srand(time(NULL));

  pthread_mutex_init(&mx_1, NULL);
  pthread_mutex_init(&mx_2, NULL);

  pthread_t tarr[TCNT];
  for (int i = 0; i < TCNT; ++i) {
    pthread_create(tarr + i, NULL, &(routine), NULL);
  }

  for (int i = 0; i < TCNT; ++i) {
    pthread_join(tarr[i], NULL);
  }

  pthread_mutex_destroy(&mx_1);
  pthread_mutex_destroy(&mx_2);
  return 0;
}

/*
 * File: 15.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-07
 * Description: recursive mutexes
 * NOTE: keep it working & simple
 */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define TCNT 2

pthread_mutex_t mutex;
int v1 = 10;

void *routine(void *args) {
  int thread_id = *((int *)args);
  pthread_mutex_lock(&mutex);
  printf("(LOCK 1) thread_id: %d begins\n", thread_id);
  pthread_mutex_lock(&mutex);
  printf("(LOCK 2) thread_id: %d begins\n", thread_id);

  printf("original: %d\n", v1);
  v1 += 10;
  printf("newer: %d\n", v1);

  // NOTE: must unlock recursive locks same number of times else deadlock
  printf("(UNLOCK 2) thread_id: %d ends\n", thread_id);
  pthread_mutex_unlock(&mutex);
  printf("(UNLOCK 1) thread_id: %d ends\n", thread_id);
  pthread_mutex_unlock(&mutex);
  return NULL;
}

int main() {

  pthread_mutexattr_t mutex_config;
  pthread_mutexattr_init(&mutex_config);
  /* NOTE: recursive mutex: same mutex locked over & over

   * {owner_thread_id, lock_count} is maintained internally
   * if unlocked: owner_thread_id = current_thread, lock_count = 1
   * if new_thread asks for lock: new_thread_id == current_thread
   * if yes: lock_count++, else block (new_thread)
   * release only happens when lock_count for owner_thread_id is ZERO
  */
  pthread_mutexattr_settype(&mutex_config, PTHREAD_MUTEX_RECURSIVE);

  // NOTE: set the configuration to the mutex HERE
  pthread_mutex_init(&mutex, &mutex_config);

  pthread_t tarr[TCNT];
  int values[TCNT];
  for (int i = 0; i < TCNT; ++i) {
    values[i] = i;
    pthread_create(tarr + i, NULL, &(routine), (void *)(&values[i]));
  }

  for (int i = 0; i < TCNT; ++i) {
    pthread_join(tarr[i], NULL);
  }

  pthread_mutexattr_destroy(&mutex_config);

  pthread_mutex_destroy(&mutex);
  return 0;
}

/*
 * File: 16.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-07
 * Description: semaphore introduction
 * NOTE: keep it working & simple
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#include <stdlib.h>

#define TCNT 16

int shared_resource = 10;
sem_t semaphore;

void *routine(void *args) {
  int thread_id = *((int *)args);
  printf("(%d) waiting\n", thread_id);

  // NOTE: similar to lock
  sem_wait(&semaphore); // P(s): while(s<=0) wait; s--;
  printf("(%d) begins\n", thread_id);

  sleep(rand() % 5 + 1);
  shared_resource++;

  printf("(%d) ends\n", thread_id);
  sem_post(&semaphore); // V(s): s++;
  // NOTE: similar to unlock

  return NULL;
}

int main() {
  srand(time(NULL));
  pthread_t tarr[TCNT];
  int values[TCNT];

  /* NOTE: this seems similar to a mutex? on a first glance, YES!
   * semaphore can take integral values > 1? multi-valued mutex? NO!
   * mutex implies ownership, semaphore does NOT imply so
   * i.e. same thread must lock & unlock before others can use
   * normal mutex multi-lock: deadlock, recursive: ok (must be same thread)
   * no such restriction on semaphores: only wait, only post, or both!
   * use-cases: resource limitation, producer-consumer, reader-writer
   *
   * NOTE: semaphore represents resource availability
   */
  sem_init(&semaphore, 0, 12);

  for (int i = 0; i < TCNT; ++i) {
    values[i] = i;
    pthread_create(tarr + i, NULL, &routine, (void *)(&values[i]));
  }

  for (int i = 0; i < TCNT; ++i) {
    pthread_join(tarr[i], NULL);
  }

  sem_destroy(&semaphore);
  return 0;
}

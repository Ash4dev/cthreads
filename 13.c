/*
 * File: 13.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-07
 * Description: Detached threads
 * NOTE: keep it working & simple
 */

#include <stdio.h>

#include <pthread.h>
#include <unistd.h>

#define TCNT 2

void *routine(void *args) {
  int thread_id = *((int *)args);
  printf("thread_id: %d begins\n", thread_id);
  sleep(1);
  printf("thread_id: %d ends\n", thread_id);
  return NULL;
}

int main() {
  printf("main thread begins\n");
  pthread_t tarr[TCNT];
  int values[TCNT];

  pthread_attr_t tconfig;
  pthread_attr_init(&tconfig);
  pthread_attr_setdetachstate(&tconfig, PTHREAD_CREATE_DETACHED);

  for (int i = 0; i < TCNT; ++i) {
    values[i] = i;

    // NOTE: such threads detach from parent thread: long running bcg work

    // pthread_create(tarr + i, NULL, &routine, (void *)&values[i]);
    // pthread_detach(tarr[i]);

    // NOTE: method 2: edge-case bwn create and detach thread ended (res loss)
    pthread_create(tarr + i, &tconfig, &routine, (void *)(&values[i]));
  }

  pthread_attr_destroy(&tconfig);

  printf("main thread ends\n");
  pthread_exit(0);
}

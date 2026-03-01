/*
 * File: 12a.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-01
 * Description: Barrier example
 * NOTE: keep it working & simple
 */

#include <stdio.h>

#include <pthread.h>

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define TCNT 8

typedef struct tpack {
  int thread_id;
  int result;
} tpack_t;

void *roll_die(void *args) {
  tpack_t *targs = (tpack_t *)args;
  int val = (rand() % 6) + 1;
  targs->result = val;
  return NULL;
}

int main() {
  srand(time(NULL));

  pthread_t tarr[TCNT];
  tpack_t tinfo[TCNT];

  for (int i = 0; i < TCNT; ++i) {
    tinfo[i].thread_id = i;
    pthread_create(tarr + i, NULL, &roll_die, (tinfo + i));
  }

  for (int i = 0; i < TCNT; ++i) {
    pthread_join(tarr[i], NULL);
  }

  tpack_t winner;
  winner.result = 0;
  winner.thread_id = 0;

  // NOTE: need all the threads to be prepared before result calculation
  // does NOT join ensure that for me already? what more can a barrier do?
  for (int i = 0; i < TCNT; ++i) {
    if (winner.result > tinfo[i].result) {
      continue;
    }
    winner.thread_id = i;
    winner.result = tinfo[winner.thread_id].result;
  }

  printf("winner: thread %d, result: %d\n", winner.thread_id, winner.result);
  return 0;
}

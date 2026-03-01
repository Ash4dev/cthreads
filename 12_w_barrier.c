/*
 * File: 12_threads_coordinate.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-01
 * Description:
 * NOTE: keep it working & simple
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TCNT 8

typedef struct {
  int thread_id;
  int result;
} tpack_t;

tpack_t winner = {0};

// NOTE: barries create phase transition
// die roll -> winner calc -> report winner
pthread_barrier_t dice_barrier;
pthread_barrier_t winner_barrier;

void *roll_die(void *args) {
  tpack_t *targs = (tpack_t *)args;
  targs->result = (rand() % 6) + 1;

  pthread_barrier_wait(&dice_barrier);
  // NOTE: once the main and these threads wait at barrier, release

  // NOTE: created threads must wait here
  pthread_barrier_wait(&winner_barrier);

  // NOTE: calculate the result of the winner inside thread
  if (targs->thread_id == winner.thread_id)
    printf("Thread %d WON with %d\n", targs->thread_id, targs->result);
  else
    printf("Thread %d LOST with %d\n", targs->thread_id, targs->result);

  return NULL;
}

int main() {
  srand(time(NULL));

  pthread_t threads[TCNT];
  tpack_t tinfo[TCNT];

  pthread_barrier_init(&dice_barrier, NULL, TCNT + 1);
  pthread_barrier_init(&winner_barrier, NULL, TCNT + 1);

  for (int i = 0; i < TCNT; i++) {
    tinfo[i].thread_id = i;
    pthread_create(&threads[i], NULL, roll_die, &tinfo[i]);
  }

  // NOTE: main waits here after thread creation, only move ahead once rolled
  pthread_barrier_wait(&dice_barrier);

  winner = tinfo[0];
  for (int i = 1; i < TCNT; i++) {
    if (tinfo[i].result > winner.result)
      winner = tinfo[i];
  }

  // NOTE: main calculates the winner while the other threads wait
  pthread_barrier_wait(&winner_barrier);

  for (int i = 0; i < TCNT; i++)
    pthread_join(threads[i], NULL);

  printf("Final Winner: Thread %d with %d\n", winner.thread_id, winner.result);

  pthread_barrier_destroy(&dice_barrier);
  pthread_barrier_destroy(&winner_barrier);

  return 0;
}

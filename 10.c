/*
 * File: 10.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-01
 * Description:
 * NOTE: keep it working & simple
 */

#include <stdio.h>

#include <pthread.h>
#include <unistd.h>

#include <stdlib.h>
#include <time.h>

#define SR_CNT 4
#define TCNT 10

int sr_amt[SR_CNT];
pthread_mutex_t sr_mxs[SR_CNT];

void *consume(void *args) {
  int thread_id = *((int *)args);
  printf("thread %d begun\n", thread_id);

  // NOTE: allows for even load distribution
  int start = rand() % SR_CNT;
  for (int offset = 0; offset < SR_CNT; ++offset) {
    int i = (start + offset) % SR_CNT;

    if (pthread_mutex_trylock(sr_mxs + i) == 0) {
      printf("thread %d -> accesses sr: %d\n", thread_id, i);

      int consumption = (rand() % 30);

      if (sr_amt[i] < consumption) {
        printf("thread %d -> ain't got enough sr: %d\n", thread_id,
               consumption);
        usleep(500 * 1000);
        pthread_mutex_unlock(sr_mxs + i);
        continue;
      }

      printf("thread %d -> begin: %d\n", thread_id, sr_amt[i]);
      printf("thread %d -> consumption %d\n", thread_id, consumption);
      printf("thread %d -> end: %d\n", thread_id, sr_amt[i] - consumption);

      sr_amt[i] -= consumption;
      usleep(500 * 1000);
      pthread_mutex_unlock(sr_mxs + i);
      goto end_thread;
    }
  }
  printf("thread %d -> NO sr found\n", thread_id);

end_thread:
  printf("thread %d ended\n", thread_id);
  return NULL;
}

int main() {
  srand(time(NULL));

  for (int i = 0; i < SR_CNT; ++i) {
    sr_amt[i] = 100;
    pthread_mutex_init(sr_mxs + i, NULL);
  }

  int values[TCNT];
  pthread_t tarr[TCNT];
  for (int i = 0; i < TCNT; ++i) {
    values[i] = i;
    pthread_create(tarr + i, NULL, &consume, (void *)(&values[i]));
  }

  for (int i = 0; i < TCNT; ++i) {
    pthread_join(tarr[i], NULL);
  }

  for (int i = 0; i < SR_CNT; ++i) {
    pthread_mutex_destroy(sr_mxs + i);
  }
}

/*
 * File: 17.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-07
 * Description: producer-consumer problem
 * NOTE: keep it working & simple
 */

// NOTE: writer-reader is NOT same as producer-consumer
// readers do NOT take away resource just read only access
// consumer takes away resource from the available units

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

#define TCNT 8
#define SIZE 10

// NOTE: simulate variety of task loads
#define P_DELAY 1
#define C_DELAY 3

int buffer[SIZE];
int count = 0;

pthread_mutex_t mx_buffer;
sem_t sem_empty_cnt, sem_full_cnt;

void *producer(void *args) {
  while (1) {
    // Produce
    int x = rand() % 100 + 1;
    sleep(P_DELAY);
    printf("Produced: %d\n", x);

    // Report
    sem_wait(&sem_empty_cnt);

    pthread_mutex_lock(&mx_buffer);

    buffer[count++] = x;

    pthread_mutex_unlock(&mx_buffer);

    sem_post(&sem_full_cnt);
  }

  return NULL;
}

void *consumer(void *args) {
  while (1) {
    // Report
    int old = -1;

    sem_wait(&sem_full_cnt);

    pthread_mutex_lock(&mx_buffer);

    // NOTE: count < SIZE guaranteed -> semaphore
    old = buffer[count - 1];
    buffer[count--] = -1;

    pthread_mutex_unlock(&mx_buffer);

    sem_post(&sem_empty_cnt);

    // Consume
    sleep(C_DELAY);
    printf("Consumption result: %d\n", old);
  }
  return NULL;
}

int main() {
  srand(time(NULL));
  pthread_mutex_init(&mx_buffer, NULL);
  sem_init(&sem_empty_cnt, 0, SIZE);
  sem_init(&sem_full_cnt, 0, 0);

  pthread_t tarr[TCNT];
  for (int i = 0; i < TCNT; ++i) {
    if (i % 3 == 0) {
      pthread_create(tarr + i, NULL, &producer, NULL);
    } else {
      pthread_create(tarr + i, NULL, &consumer, NULL);
    }
  }

  for (int i = 0; i < TCNT; ++i) {
    pthread_join(tarr[i], NULL);
  }

  sem_destroy(&sem_empty_cnt);
  sem_destroy(&sem_full_cnt);
  pthread_mutex_destroy(&mx_buffer);
  return 0;
}

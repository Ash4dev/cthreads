/*
 * File: 9_w_wait.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-02-28
 * Description: Conditional variables
 * NOTE: keep it working & simple
 */

#include <stdio.h>

#include <pthread.h>
#include <unistd.h>

#define TCNT 8
#define PUMP_LIMIT_MIN 0
#define PUMP_LIMIT_MAX 100

#define PUMP_WAIT_ITER 5
#define PUMP_IN_RATE 25
#define PUMP_OUT_RATE 5

int pump_fuel = 0;
pthread_mutex_t mx_fuel;
// NOTE: Condition variables must always be used with a mutex protecting it
pthread_cond_t cnd_fuel;

/* NOTE:
 * previous program: producers/consumers early exit if condition fails
 * current program: consumers wait till their turn comes
 */

void *obtain_fuel(void *args) {
  int thread_id = *((int *)args);

  if ((pthread_mutex_lock(&mx_fuel) == 0)) {
    printf("thread %d begins\n", thread_id);

    for (int i = 0;
         (pump_fuel + PUMP_IN_RATE <= PUMP_LIMIT_MAX) && i < PUMP_WAIT_ITER;
         ++i) {
      pump_fuel += PUMP_IN_RATE;

      /*
       * let waiting threads know they can consume when just enough
       * control transfer is not immediate, one waiting -> runnable
       * that thread cannot proceed until mutex is freed by current
       *
       * if mutex is acquired before for loop & released after it
       * early signalling feels useless, need to unlock after signal (burst)
       *
       * but if goto unlock is called to allow for just enough filling
       * but that would mean it will fill that is just enough for any thread
       * never fully filled, approach: determine excess limit (statistics)
       */
      if (pump_fuel >= PUMP_OUT_RATE * PUMP_WAIT_ITER) {
        // signal: identical threads, boradcast: if different conditions
        pthread_cond_signal(&cnd_fuel);
        // goto unlock; // NOTE: design decision
      }
    }

  unlock:
    printf("pump_fuel: %d @ thread_id: %d\n", pump_fuel, thread_id);
    printf("thread %d ends\n", thread_id);
    pthread_mutex_unlock(&mx_fuel);
  }

  return NULL;
}

void *give_out_fuel(void *args) {
  int thread_id = *((int *)args);

  if ((pthread_mutex_lock(&mx_fuel) == 0)) {
    printf("thread %d begins\n", thread_id);

    while (pump_fuel <= PUMP_LIMIT_MIN) {
      printf("waiting theading %d\n", thread_id);
      // NOTE: recovery: when consumer is ready before producer (1 shot)

      /* NOTE: naive wait
       *
       * sleep(1)
       * goto unlock; // if not DEADLOCK, yes -> immediate exit
       */

      // while loop is a must to avoid spurious acquisition
      pthread_cond_wait(&cnd_fuel, &mx_fuel);
      // equivalent to: unlock -> wait -> lock
    }

    for (int i = 0;
         (pump_fuel - PUMP_OUT_RATE >= PUMP_LIMIT_MIN) && i < PUMP_WAIT_ITER;
         ++i) {
      pump_fuel -= PUMP_OUT_RATE;
    }

  unlock:
    printf("pump_fuel: %d @ thread_id: %d\n", pump_fuel, thread_id);
    printf("thread %d ends\n", thread_id);
    pthread_mutex_unlock(&mx_fuel);
  }

  return NULL;
}

int main() {
  pthread_mutex_init(&mx_fuel, NULL);
  pthread_cond_init(&cnd_fuel, NULL);

  pthread_t tarr[TCNT];

  int values[TCNT];
  for (int i = 0; i < TCNT; ++i) {
    values[i] = i;
  }

  pthread_create(tarr + 0, NULL, &give_out_fuel, (void *)(&values[0]));
  for (int i = 1; i < TCNT; ++i) {
    if (i % 2 == 0) {
      // NOTE: minimize recoveries by fixed time refuelling
      pthread_create(tarr + i, NULL, &obtain_fuel, (void *)(&values[i]));
    }
    pthread_create(tarr + i, NULL, &give_out_fuel, (void *)(&(values[i])));
  }

  for (int i = 0; i < TCNT; ++i) {
    pthread_join(tarr[i], NULL);
  }

  pthread_cond_destroy(&cnd_fuel);
  pthread_mutex_destroy(&mx_fuel);
  return 0;
}

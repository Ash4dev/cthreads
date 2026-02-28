/*
 * File: 9.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-02-28
 * Description: Conditional variables
 * NOTE: keep it working & simple
 */

#include <stdio.h>

#include <pthread.h>
#include <time.h>

#define TCNT 5
#define PUMP_LIMIT_MIN 0
#define PUMP_LIMIT_MAX 100

int pump_fuel = 0;
pthread_mutex_t mx_fuel;

// NOTE: producer wrt pump_fuel
void *obtain_fuel(void *args) {
  int thread_id = *((int *)args);

  if ((pthread_mutex_lock(&mx_fuel) == 0)) {
    printf("thread %d begins\n", thread_id);

    // NOTE: check inside & NOT expression (eval order implm specific)
    if (pump_fuel >= PUMP_LIMIT_MAX) {
      // NOTE: in case of early exit, always UNLOCK! (else DEADLOCK)
      goto unlock;
    }

    for (int i = 0; (pump_fuel + 10 <= PUMP_LIMIT_MAX) && i < 5; ++i) {
      pump_fuel += 10;
    }

    printf("pump_fuel: %d @ thread_id: %d\n", pump_fuel, thread_id);

    printf("thread %d ends\n", thread_id);
  unlock:
    pthread_mutex_unlock(&mx_fuel);
  }

  return NULL;
}

// NOTE: consumer wrt pump_fuel
void *give_out_fuel(void *args) {
  int thread_id = *((int *)args);

  if ((pthread_mutex_lock(&mx_fuel) == 0)) {
    printf("thread %d begins\n", thread_id);

    if (pump_fuel <= PUMP_LIMIT_MIN) {
      goto unlock;
    }

    for (int i = 0; (pump_fuel - 5 >= PUMP_LIMIT_MIN) && i < 5; ++i) {
      pump_fuel -= 5;
    }

    printf("pump_fuel: %d @ thread_id: %d\n", pump_fuel, thread_id);

    printf("thread %d ends\n", thread_id);
  unlock:
    pthread_mutex_unlock(&mx_fuel);
  }

  return NULL;
}

int main() {
  pthread_mutex_init(&mx_fuel, NULL);

  pthread_t tarr[TCNT];
  int values[TCNT] = {0};

  // NOTE: 1 producer & 4 consumers wrt pump_fuel
  pthread_create(tarr + 0, NULL, &obtain_fuel, (void *)(&values[0]));
  for (int i = 1; i < TCNT; ++i) {
    values[i] = i;
    pthread_create(tarr + i, NULL, &give_out_fuel, (void *)(&values[i]));
  }

  for (int i = 0; i < TCNT; ++i) {
    pthread_join(tarr[i], NULL);
  }

  pthread_mutex_destroy(&mx_fuel);
  return 0;
}

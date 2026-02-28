/*
 * File: 7.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-02-27
 * Description: Obtain sum of array dividing into subproblems
 * NOTE: keep it working & simple
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define TCNT 2

int primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41};

typedef struct threadargs {
  size_t size;
  int start;
  int result;
} targs_t;

void *sum_with_threads(void *args) {
  targs_t *threadarg = ((targs_t *)args);
  printf("begin\t");

  int sum = 0;
  for (int i = threadarg->start; i < threadarg->start + threadarg->size; i++) {
    usleep(100 * 1000);
    printf("%d\t", primes[i]);
    sum += primes[i];
  }
  threadarg->result = sum;

  printf("end\n");
  return NULL;
}

int main() {
  int arrcnt = sizeof(primes) / sizeof(int);
  int multiplier = (arrcnt + TCNT - 1) / TCNT;
  pthread_t tarr[TCNT];

  int runningSum = 0;
  targs_t targs[TCNT] = {0};

  for (int i = 0; i < TCNT; i++) {
    targs[i].result = 0;
    targs[i].start = multiplier * i;

    int potsize1 = multiplier;
    int potsize2 = (arrcnt - targs[i].start);
    targs[i].size = (potsize1 < potsize2) ? potsize1 : potsize2;

    pthread_create(tarr + i, NULL, &sum_with_threads, &targs[i]);
  }

  for (int i = 0; i < TCNT; i++) {
    pthread_join(tarr[i], NULL);
    runningSum += targs[i].result;
  }

  printf("running sum: %d\n", runningSum);
  return 0;
}

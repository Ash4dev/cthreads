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
} targs_t;

void *sum_with_threads(void *args) {
  targs_t *targs = ((targs_t *)args);
  printf("begin\t");

  // NOTE: allocation 2
  int *sum = malloc(sizeof(*sum));
  if (sum == NULL) {
    free(args);
    return NULL;
  }

  *sum = 0;
  for (int i = targs->start; i < targs->start + targs->size; i++) {
    usleep(100 * 1000);
    printf("%d\t", primes[i]);
    *sum += primes[i];
  }

  // NOTE: deallocation 1
  free(args);
  args = NULL;
  targs = NULL;
  printf("end\n");
  return (void *)sum;
}

// NOTE: code produces the correct result but (de)allocations are all around

int main() {
  int arrcnt = sizeof(primes) / sizeof(int);
  int multiplier = (arrcnt + TCNT - 1) / TCNT;
  pthread_t tarr[TCNT];

  int runningSum = 0;

  for (int i = 0; i < TCNT; i++) {
    // NOTE: allocation 1 here
    targs_t *args = malloc(sizeof(*args));
    if (args == NULL) {
      continue;
    }
    args->start = multiplier * i;

    int potsize1 = multiplier;
    int potsize2 = (arrcnt - args->start);
    args->size = (potsize1 < potsize2) ? potsize1 : potsize2;

    pthread_create(tarr + i, NULL, &sum_with_threads, args);
    // NOTE: cannot deallocate here since args may not have been used already
  }

  for (int i = 0; i < TCNT; i++) {
    int *res;
    pthread_join(tarr[i], (void **)&res);
    if (res == NULL) {
      continue;
    }
    runningSum += *(res);
    // NOTE: deallocation 2
    free(res);
    res = NULL;
  }

  printf("running sum: %d\n", runningSum);
  return 0;
}

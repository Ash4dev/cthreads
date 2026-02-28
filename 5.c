/*
 * File: 5.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-02-26
 * Description: thread return value
 * NOTE: keep it working & simple
 */

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <pthread.h>

void *some_fn(void *args) {
  // NOTE: cannot return value as stack allocated
  // if we do not wish to use pointers, can we use static local variable

  // NOTE: NOT a good idea to allocate & deallocate at separate places
  int *value = malloc(sizeof *value);
  *value = (rand() % 6) + 1;

  if (!value) {
    pthread_exit(NULL);
  }
  return (void *)value;
}

// NOTE: check the issue with valgrind

int main() {
  // NOTE: initialize with a random seed
  srand(time(NULL));

  pthread_t tarr[3];
  int *tres[3] = {0};

  for (int i = 0; i < 3; i++) {
    pthread_create(tarr + i, NULL, &some_fn, NULL);
  }

  for (int i = 0; i < 3; i++) {
    // NOTE: tres pointer constant to the first element
    // tres + i: pointer arithmetic -> temp memory address

    pthread_join(tarr[i], (void **)(&tres[i])); // tres[i] captures
    printf("result of thread %d is %d\n", i, *(tres[i]));
  }

  for (int i = 0; i < 3; i++) {
    free(tres[i]);
  }
  return 0;
}

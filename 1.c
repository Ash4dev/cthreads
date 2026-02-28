/*
 * File: 1.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-02-25
 * Description:
 * NOTE: keep it working & simple
 */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

// NOTE: thread func must take 1 void* argument and have void* return type
void *func(void *args) {
  printf("Begin. Waiting for %d\n", *(int *)args / 1000);
  sleep(*(int *)args / 1000); // [unistd.h]
  printf("Fn just for the sake of it\n");
  printf("End\n");
  return (void *)10;
}

// NOTE: pthread shared library must be linked each time gcc -pthread ....

int main() { // NOTE: main thread begins here (not a special thread)
  int x = 10;
  char ch = 'A';

  // NOTE: threads share heap, global variables, file descriptors
  // but have indepenedent stack, registers, program counter
  pthread_t t1, t2;

  // NOTE: basic unit of execution in thread is fn: indpt exec begins & stops
  pthread_create(&t1, NULL, &func, (void *)&x);
  pthread_create(&t2, NULL, &func, (void *)&ch);

  // NOTE: try to keep each threads task as indepenedent as possible,
  // it is NOT possible to do each time. in those cases, mechanisms like
  // synchronization are needed. mutex, conditional variables, atomic

  // NOTE: if not done, makes it a zombie thread (resources NOT released)
  pthread_join(t1, NULL); // NOTE: similar to wait() for processes
  pthread_join(t2, NULL);

  // NOTE: each has return value zero to mean success (easier to check fail)
  return 0;
}

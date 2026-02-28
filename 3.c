/*
 * File: 3.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-02-26
 * Description: Race conditions
 * NOTE: keep it working & simple
 */
#include <pthread.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

// NOTE: shared_resource can be anything, for simplicity an int here
int shared_resource = 0;

void *resource_counter(void *args) {
  for (int i = 0; i < 1000000; i++) {
    shared_resource++;
    // NOTE: assembly instructions: read -> copy of old -> increment
    // OS interleaves the operations to perform fast, (1)r | (2) r,c,w| (1) c,w
    // it is NOT guaranteed to be sequential: tradeoff for parallelness
    // this is a RACE condition
  }
  return NULL;
}

int main() {
  pthread_t t1, t2;

  pid_t child = fork();
  if (child < 0) {
    return -1;
  } else if (child == 0) {
    printf("child begins with pid: %d\n", getpid());
  } else {
    printf("parent begins with pid: %d\n", getpid());
    wait(NULL);
  }

  // NOTE: resource_counter mimics big function (still runs) when t2 begins
  // (more interleaves)
  if (pthread_create(&t1, NULL, &resource_counter, NULL)) {
    return 1;
  }

  if (pthread_create(&t2, NULL, &resource_counter, NULL)) {
    return 2;
  }

  if (pthread_join(t1, NULL)) {
    return 3;
  }

  if (pthread_join(t2, NULL)) {
    return 3;
  }

  printf("in pid: %d, value of shared_resource: %d\n", getpid(),
         shared_resource);
  return 0;
}

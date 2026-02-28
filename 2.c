/*
 * File: 2.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-02-26
 * Description:
 * NOTE: keep it working & simple
 */

// NOTE: Codevault Processes playlist (needs to be done parallely)
// https://www.youtube.com/playlist?list=PLfqABt5AS4FkW5mOn2Tn9ZZLLDwA3kZUY

#include <stdio.h>

#include <pthread.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int x = 1;

// NOTE: both the threads seem to use a shared resource: int x
// the program has a data race -> no sycnchronization (UB)
void *thread_func_1(void *args) {
  ++x;
  sleep(2);
  printf("process id: %d, x is %d\n", getpid(), x);
  return NULL;
}

void *thread_func_2(void *args) {
  sleep(2);
  printf("process id: %d, x is %d\n", getpid(), x);
  return NULL;
}

int main() {

  // NOTE: copies all of the process state (throughout) to create a child pro

  pid_t pid = fork();
  if (pid < 0) {
    fprintf(stderr, "fork failed\n");
    return 1;
  } else if (pid == 0) {
    printf("I'm the child process. x is %d\n", ++x);
    printf("child pid: %d, parent pid: %d\n", getpid(), getppid());
  } else {
    sleep(2);
    printf("i'm the parent process. x is %d\n", x);
    printf("parent pid: %d, child pid: %d\n", getpid(), pid);

    // NOTE: blocks par pro until at least one of the child pro terminates
    // why NOT wait for all the child process to terminate?
    // in case of multiple children processes: while (wait(NULL) > 0)
    wait(NULL);
  }

  // NOTE: both the parent and child create 2 threads of their own
  pthread_t t1, t2;
  pthread_create(&t1, NULL, &thread_func_1, NULL);
  pthread_create(&t2, NULL, &thread_func_2, NULL);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  // NOTE: most significant diff: memory address space sharing -> independence
  // process: no sharing at all between par & child (explicit hierarchy),
  // thread: share heap, static/global, code variables but NOT stack, registers
  return 0;
}

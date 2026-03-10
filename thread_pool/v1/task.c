/*
 * File: task.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-09
 * Description:
 * NOTE: keep it working & simple
 */

#include "task_queue.h"

#include <stdio.h>
#include <stdlib.h>

#define TCNT 6
#define TASK_COUNT 500

typedef struct user_task_1 {
  int a;
  int b;
  int res;
} user_task_1;

void *routine_task_1(void *args) {
  user_task_1 *targs = (user_task_1 *)args;
  targs->res = targs->a + targs->b;
  printf("inside: %d + %d = %d\n", targs->a, targs->b, targs->res);
  return NULL;
}

int main() {
  pthread_t tarr[TCNT];
  initialize_sync_primitives();
  for (int i = 0; i < TCNT; ++i) {
    pthread_create(tarr + i, NULL, worker, NULL);
  }

  srand(time(NULL));

  user_task_1 argsarr[TASK_COUNT];
  task_t tasksarr[TASK_COUNT];

  for (int i = 0; i < TASK_COUNT; ++i) {
    user_task_1 t1;
    argsarr[i] = t1;
    argsarr[i].a = rand() % 100 + 1;
    argsarr[i].b = rand() % 100 + 1;

    task_t t;
    tasksarr[i] = t;
    tasksarr[i].fnc = &routine_task_1;
    tasksarr[i].args = (void *)(&argsarr[i]);

    submit_task_to_queue(&tasksarr[i]);
  }

  for (int i = 0; i < TCNT; ++i) {
    pthread_join(tarr[i], NULL);
  }

  for (int i = 0; i < TASK_COUNT; ++i) {
    printf("final: %d + %d = %d\n", argsarr[i].a, argsarr[i].b, argsarr[i].res);
  }

  destroy_sync_primitives();
  return 0;
}

/*
 * File: main.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-10
 * Description:
 * NOTE: keep it working & simple
 */

#include "thread_pool.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int a;
  int b;
} add_task;

void add(void *arg) {
  add_task *t = arg;
  printf("%d + %d = %d\n", t->a, t->b, t->a + t->b);
  free(t);
}

int main() {
  thread_pool_t pool;

  thread_pool_init(&pool, 6);

  for (int i = 0; i < 50; i++) {
    add_task *t = malloc(sizeof(add_task));

    t->a = rand() % 100;
    t->b = rand() % 100;

    thread_pool_submit(&pool, add, t);
  }

  thread_pool_shutdown(&pool);
}

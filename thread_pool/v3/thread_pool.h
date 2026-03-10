#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>

typedef struct task {
  void (*fnc)(void *);
  void *arg;

  // NOTE: TASKS MODELLED AS LINKED LIST
  struct task *next;
} task_t;

typedef struct {
  pthread_mutex_t mutex;
  pthread_cond_t cond;

  // NOTE: ACTUAL SHARED RESOURCE
  task_t *head;
  task_t *tail;

  pthread_t *workers;
  int worker_count;

  int shutdown;
} thread_pool_t;

void thread_pool_init(thread_pool_t *pool, int workers);
void thread_pool_submit(thread_pool_t *pool, void (*fnc)(void *), void *arg);
void thread_pool_shutdown(thread_pool_t *pool);

#endif

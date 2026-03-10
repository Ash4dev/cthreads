/*
 * File: thread_pool.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-10
 * Description:
 * NOTE: keep it working & simple
 */

#include "thread_pool.h"
#include <stdlib.h>

static void *worker(void *arg);

void thread_pool_init(thread_pool_t *pool, int workers) {
  pool->worker_count = workers;
  pool->shutdown = 0;

  pool->head = NULL;
  pool->tail = NULL;

  pthread_mutex_init(&pool->mutex, NULL);
  pthread_cond_init(&pool->cond, NULL);

  pool->workers = malloc(sizeof(pthread_t) * workers);

  for (int i = 0; i < workers; i++)
    pthread_create(pool->workers + i, NULL, worker, pool);
}

void thread_pool_submit(thread_pool_t *pool, void (*fnc)(void *), void *arg) {
  task_t *task = malloc(sizeof(task_t));

  task->fnc = fnc;
  task->arg = arg;
  task->next = NULL;

  pthread_mutex_lock(&pool->mutex);

  if (pool->tail) {
    // NON-EMPTY LINKED LIST
    pool->tail->next = task;
  } else {
    pool->head = task;
  }

  // MARK CURRENT TASK AS TAIL
  pool->tail = task;

  // NOTE: TASK IS AVAILABLE (WORKER CAN PROCEED)
  pthread_cond_signal(&pool->cond);

  pthread_mutex_unlock(&pool->mutex);
}

static void *worker(void *arg) {
  thread_pool_t *pool = arg;

  while (1) {
    pthread_mutex_lock(&pool->mutex);

    // NOTE: NO WORK CHECK
    while (pool->head == NULL && !pool->shutdown) {
      // UNLOCK -> SLEEP -> ON WAKE, RE-LOCK
      pthread_cond_wait(&pool->cond, &pool->mutex);
    }

    // NOTE: COMPLETE PENDING TASKS IF SHUTDOWN INITIATED
    if (pool->shutdown && pool->head == NULL) {
      pthread_mutex_unlock(&pool->mutex);
      break;
    }

    task_t *task = pool->head;
    pool->head = task->next;

    // EMPTY LINKED LIST
    if (pool->head == NULL) {
      pool->tail = NULL;
    }

    pthread_mutex_unlock(&pool->mutex);

    // NOTE: EXECUTE TASK OUTSIDE LOCK (AVOID SERIALIZATION)
    task->fnc(task->arg);
    free(task);
  }

  return NULL;
}

void thread_pool_shutdown(thread_pool_t *pool) {
  pthread_mutex_lock(&pool->mutex);
  pool->shutdown = 1;
  pthread_cond_broadcast(&pool->cond);
  pthread_mutex_unlock(&pool->mutex);

  for (int i = 0; i < pool->worker_count; i++)
    pthread_join(pool->workers[i], NULL);

  free(pool->workers);

  pthread_mutex_destroy(&pool->mutex);
  pthread_cond_destroy(&pool->cond);
}

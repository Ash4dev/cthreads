/*
 * File: thread_pool.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-10
 * Description:
 * NOTE: keep it working & simple
 */

#include "thread_pool.h"

task_t queue[TASK_QUEUE_MAX_SIZE];

int head = 0;
int tail = 0;

pthread_mutex_t mx;
sem_t sem_empty;
sem_t sem_full;

pthread_t workers[WORKER_COUNT];

void execute_task(task_t task) {
  if (task.fnc == NULL)
    pthread_exit(NULL);

  task.fnc(task.arg);
}

void *worker(void *arg) {
  while (1) {
    sem_wait(&sem_full);

    pthread_mutex_lock(&mx);

    task_t task = queue[head];
    head = (head + 1) % TASK_QUEUE_MAX_SIZE;

    pthread_mutex_unlock(&mx);

    sem_post(&sem_empty);

    execute_task(task);
  }
}

void thread_pool_init() {
  pthread_mutex_init(&mx, NULL);

  sem_init(&sem_empty, 0, TASK_QUEUE_MAX_SIZE);
  sem_init(&sem_full, 0, 0);

  for (int i = 0; i < WORKER_COUNT; i++)
    pthread_create(&workers[i], NULL, worker, NULL);
}

void thread_pool_submit(task_t *task) {
  sem_wait(&sem_empty);

  pthread_mutex_lock(&mx);

  queue[tail] = *task;
  tail = (tail + 1) % TASK_QUEUE_MAX_SIZE;

  pthread_mutex_unlock(&mx);

  sem_post(&sem_full);
}

void thread_pool_shutdown() {
  task_t poison = {NULL, NULL};

  for (int i = 0; i < WORKER_COUNT; i++)
    thread_pool_submit(&poison);

  for (int i = 0; i < WORKER_COUNT; i++)
    pthread_join(workers[i], NULL);

  pthread_mutex_destroy(&mx);
  sem_destroy(&sem_empty);
  sem_destroy(&sem_full);
}

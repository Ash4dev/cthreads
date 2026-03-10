#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include <semaphore.h>

#define TASK_QUEUE_MAX_SIZE 256
#define WORKER_COUNT 6

typedef struct task {
  void (*fnc)(void *);
  void *arg;
} task_t;

extern task_t task_queue[];
extern int head, tail; // en: rear-add, de: front-del

extern pthread_mutex_t mx_task_queue;

extern sem_t sem_task_queue_fill_cnt;
extern sem_t sem_task_queue_empty_cnt;

void thread_pool_init();
void thread_pool_submit(task_t *task);
void thread_pool_shutdown();

#endif

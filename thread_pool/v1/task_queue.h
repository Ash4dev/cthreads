/*
 * File: task_queue.h
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-09
 * Description: Thread Pools
 * NOTE: keep it working & simple
 */

#include <pthread.h>
#include <semaphore.h>

#define TASK_QUEUE_MAX_SIZE 256

typedef struct task {
  void *(*fnc)(void *);
  void *args;
} task_t;

extern task_t task_queue[];
extern int enqueue, dequeue; // en: rear-add, de: front-del

extern pthread_mutex_t mx_task_queue;

extern sem_t sem_task_queue_fill_cnt;
extern sem_t sem_task_queue_empty_cnt;

void initialize_sync_primitives();
void destroy_sync_primitives();
void submit_task_to_queue(task_t *task);
void execute_candidate_task();
void *worker(void *args);

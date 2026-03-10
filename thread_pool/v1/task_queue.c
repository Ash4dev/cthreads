/*
 * File: task_queue.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-03-09
 * Description:
 * NOTE: keep it working & simple
 */

#include "task_queue.h"
#include <stdio.h>

int enqueue;
int dequeue;

pthread_mutex_t mx_task_queue;

sem_t sem_task_queue_fill_cnt;
sem_t sem_task_queue_empty_cnt;
task_t task_queue[TASK_QUEUE_MAX_SIZE];

void initialize_sync_primitives() {
  sem_init(&sem_task_queue_empty_cnt, 0, TASK_QUEUE_MAX_SIZE);
  sem_init(&sem_task_queue_fill_cnt, 0, 0);

  enqueue = dequeue = 0;

  pthread_mutex_init(&mx_task_queue, NULL);
}

void destroy_sync_primitives() {
  sem_destroy(&sem_task_queue_empty_cnt);
  sem_destroy(&sem_task_queue_fill_cnt);

  pthread_mutex_destroy(&mx_task_queue);
}

void submit_task_to_queue(task_t *task) {
  sem_wait(&sem_task_queue_empty_cnt);

  pthread_mutex_lock(&mx_task_queue);

  task_queue[enqueue] = *task;
  enqueue = (enqueue + 1) % TASK_QUEUE_MAX_SIZE;

  pthread_mutex_unlock(&mx_task_queue);

  sem_post(&sem_task_queue_fill_cnt);
}

void execute_candidate_task() {
  sem_wait(&sem_task_queue_fill_cnt);

  pthread_mutex_lock(&mx_task_queue);

  task_t candidate_task = task_queue[dequeue];
  dequeue = (dequeue + 1) % TASK_QUEUE_MAX_SIZE;

  pthread_mutex_unlock(&mx_task_queue);

  sem_post(&sem_task_queue_empty_cnt);
  candidate_task.fnc(candidate_task.args);
}

void *worker(void *args) {
  while (1) {
    execute_candidate_task();
  }
}

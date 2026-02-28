
/*
 * File: 4.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-02-26
 * Description: Race Conditions & mutexes
 * NOTE: keep it working & simple
 */

#include <stdio.h>

#include <pthread.h>

// NOTE: simple mock up of a shared_resource
int shared_resource = 0;

// NOTE: naive -> boolean variable
pthread_mutex_t lock;

/*
 * hardware can execute only a single instruction at a moment
 * cpu: processors vs core
 * processors: physical chip
 * cores: logical independent divisions in the chip
 * single processor multi core: norm (cost effective)
 * less heat -> less cooling -> less components -> less cost
 *
 * NOTE: thoughts about mutexes
 *
 * mutexes do NOT allow simultaneous access to shared_resource
 * guarantee this by locking the resource from other threads
 * proper sharing is ensured, but at the cost of parallel execution
 * effectively slows down the code, avoid wherever one can
 * hence, the popularity of lock free programming
 *
 * */

void *resource_manipulator(void *args) {
  int limit = 1000 * 1000;
  for (int i = 0; i < limit; i++) {
    // NOTE: critical section begin

    // restrict access to other thread if some thread is using it
    // if (lock != 0) { wait for some time } BUT how long?
    // simple: polling at regular intervals -> done yet?

    // inform other threads that thread x is using it
    // lock = x;

    pthread_mutex_lock(&lock);

    shared_resource++; // prone to race condition

    // free up claim for others to access the critical section
    // lock = 0;
    pthread_mutex_unlock(&lock);

    // NOTE: critical section ends
  }
  return NULL;
}

int main() {

  // NOTE:
  pthread_mutex_init(&lock, NULL);

  // NOTE: beneficial number of threads is dependent on the number of cores
  // if a lot of threads are created, then the excess wuld be sequentially done
  pthread_t thread_arr[3];

  // NOTE: never create and join in the same loop -> makes code sequential!
  for (int i = 0; i < 3; i++) {
    if (pthread_create(thread_arr + i, NULL, &resource_manipulator, NULL)) {
      return i;
    }
    printf("thread %d has begun execution\n", i);
  }

  for (int i = 0; i < 3; i++) {
    if (pthread_join(thread_arr[i], NULL)) {
      return i;
    }
    printf("thread %d has been freed of its resources\n", i);
  }

  /*
   * output: b0, b1, b2, e0, e1, e2
   * parallel but in order since the same function is being executed
   * if each thread were executing different functions, may be different
   * final execution order control remains with the scheduler
   *
   * but wait, join still happens from 0 to size array? always the same?
   * if thread 2 has finished earlier, resource clean up happens only after 0
   * and thread 2 is terminated awaiting join.
   *
   */

  printf("count of shared_resource: %d\n", shared_resource);

  // NOTE:
  pthread_mutex_destroy(&lock);

  return 0;
}

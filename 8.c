#include <stdio.h>

#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex_lock;

void *routine(void *args) {
  int value = *((int *)args);
  printf("begin thread\n");

  // NOTE: trylock is non-blocking unlike pthread_mutex_lock
  // lock: threads in blocked state, trylock: remain runnable
  if (pthread_mutex_trylock(&mutex_lock)) {
    printf("thread %d did NOT get access\n", value);
    goto end;
  }

  sleep(1);
  printf("critical section access with thread %d\n", value);
  pthread_mutex_unlock(&mutex_lock);

end:
  printf("end thread\n");
  return NULL;
}

int main() {
  pthread_t tarr[4];
  int values[4];

  pthread_mutex_init(&mutex_lock, NULL);

  for (int i = 0; i < 4; ++i) {
    values[i] = i;
    // NOTE: unlike C++, there is no option of lifetime extension of r-value
    if (pthread_create(tarr + i, NULL, &routine, (void *)(&values[i]))) {
      continue;
    }
  }

  for (int i = 0; i < 4; ++i) {
    if (pthread_join(tarr[i], NULL)) {
      continue;
    }
  }

  pthread_mutex_destroy(&mutex_lock);
  return 0;
}

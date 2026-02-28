/*
 * File: 6.c
 * Author: Ashutosh Panigrahy
 * Created: 2026-02-26
 * Description:
 * NOTE: keep it working & simple
 */

#include <stdio.h>

#include <pthread.h>
#include <time.h>

#define CNT 10
int primes[CNT] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};

int indices[CNT] = {0};

void *print_a_prime(void *args) {
  int index = *((int *)args);
  printf("prime: %d\n", primes[index]);
  return NULL;
}

int main() {

  pthread_t tarr[CNT];

  for (int i = 0; i < CNT; i++) {
    indices[i] = i;
    // NOTE: i is initialized only once, while loop exactly equivalent
    // address for indices[i] is unchanging since global (static duration)
    // OR create a pointer for index before and free after use in print_a_prime
    pthread_create(tarr + i, NULL, &print_a_prime, (void *)&(indices[i]));
  }

  for (int i = 0; i < CNT; i++) {
    pthread_join(tarr[i], NULL);
  }

  return 0;
}

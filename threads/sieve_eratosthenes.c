#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

bool *all_numbers;
long long *primes;
long long primes_size;
long long primes_capacity;

struct arguments {
  long long thread_start;
  long long thread_finish;
  long long global_start;
};

long long minim(long long a, long long b) {
  if (a < b) {
    return a;
  }
  return b;
}

void* f(void* p) {
  struct arguments* thread_args = (struct arguments*) p;
  for (long long i = thread_args->thread_start; i <= thread_args->thread_finish; i++) {
    long long temp = 0;
    bool is_prime = true;
    while (temp < primes_size && primes[temp] * primes[temp] <= i) {
      if (i % primes[temp] == 0) {
        is_prime = false;
        break;
      }
      temp++;
    }
    all_numbers[i - thread_args->global_start] = is_prime;
  }
  return NULL;
}

int main(int argc, char** argv) {
  long long threads_number = 0;
  int i = 0;
  while (argv[1][i] != '\0') {
    threads_number = threads_number * 10 + (long long)(argv[1][i] - '0');
    i++;
  }

  pthread_t threads[threads_number];

  long long n = 0;
  if (argc == 3) {
    n = atoi(argv[2]);
  } else {
    n = LLONG_MAX;
  }

  primes = malloc(4 * sizeof(long long));
  primes_size = 4;
  primes_capacity = 4;
  primes[0] = 2;
  primes[1] = 3;
  primes[2] = 5;
  primes[3] = 7;

  for (int i = 0; i < primes_size && primes[i] <= n; i++) {
    printf("%lld\n", primes[i]);
  }

  all_numbers = malloc(1 * sizeof(bool));
  long long start = 11;
  long long finish;
  long long thread_length;
  long long length = 10000;
  while (start <= n) {
    if (start < length) {
      finish = minim((start - 1) * (start - 1), n);
    } else {
      finish = minim(start + length, n);
    }
    thread_length = (finish - start) / threads_number;
    
    free(all_numbers);
    all_numbers = malloc((finish - start + 1) * sizeof(bool));
    if (!all_numbers) {
      perror(NULL);
      exit(1);
    }

    struct arguments thread_args[threads_number];
    for (long long i = 0; i < threads_number; i++) {

      thread_args[i].thread_start = start + thread_length * i;
      if (i == threads_number - 1) {
        thread_args[i].thread_finish = finish;
      } else {
        thread_args[i].thread_finish = start + thread_length * (i + 1) - 1;
      }
      thread_args[i].global_start = start;
      pthread_create(&threads[i], NULL, f, (void *) &thread_args[i]);
    }

    for (long long i = 0; i < threads_number; i++){
      pthread_join(threads[i], NULL);
    }

    for (long long i = start; i <= finish; i++) {
      if (all_numbers[i - start] == true) {
        if (primes_size == primes_capacity) {
          primes = realloc(primes, primes_size * 2 * sizeof(long long));
          if (!primes) {
            perror(NULL);
            exit(1);
          }
          primes_capacity *= 2;
        }

        printf("%lld\n", i);
        primes[primes_size] = i;
        primes_size++;
      }
    }
    start = finish + 1;
  }
  free(all_numbers);
  free(primes);
  return 0;
}

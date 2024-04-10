#include <omp.h>
#include <stdio.h>

int main() {
  int message = 0;
  int num_threads = 6;
  omp_set_num_threads(num_threads);

  #pragma parallel for
    for (int i = 0; i < num_threads; i++) {
      #pragma omp critical
      {
        if (i != 0) {
          printf("Thread %d received message: %d\n", i, message);
        }

        message++;

        if (i != num_threads - 1) {
          printf("Thread %d passing message %d to Thread %d\n", i, message, (i + 1) % num_threads);
        }
      }
    }

  return 0;
}


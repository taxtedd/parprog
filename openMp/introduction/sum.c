#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  double sum = 0.0, result = 0.0;
  int n = atoi(argv[1]);

  #pragma omp parallel for
    for (int i = 1; i <= n; i++) {
      sum += 1.0 / i;
    }
  #pragma omp critical
    result += sum;

  printf("Sum: %f\n", result);
  return 0;
}
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define THREADS_NUM 4
#define N 1000

int A[N][N];
int B[N][N];
int C[N][N];

void init_matrix() {
  srand(time(NULL));

  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; j++) {
      A[i][j] = rand()%100;
      B[i][j] = rand()%100;
    }
  }
}

void parallel_multiply() {
  int i, j, k;
  double start, end;
  omp_set_num_threads(THREADS_NUM);

  start = omp_get_wtime();

#pragma omp parallel for private(i, j, k) shared(A, B, C)
  for (i = 0; i < N; ++i) {
    for (j = 0; j < N; ++j) {
      for (k = 0; k < N; ++k) {
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }

  end = omp_get_wtime();
  printf("parallel time: %f sec\n", end - start);
}

void opt_parallel_multiply() {
  int i, j, k, s;
  double start, end;
  omp_set_num_threads(THREADS_NUM);

  start = omp_get_wtime();

#pragma omp parallel for private(i, j, k, s) shared(A, B, C)
  for (i = 0; i < N; ++i) {
    for (j = 0; j < N; ++j) {
      s=0;
      #pragma omp simd
        for (k = 0; k < N; ++k) {
          s += A[i][k] * B[k][j];
        }
      C[i][j] = s;
    }
  }

  end = omp_get_wtime();
  printf("optimized parallel time: %f sec\n", end - start);
}

void sequential_multiply() {
  double start, end;
  start = omp_get_wtime();

  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {

      for (int k = 0; k < N; ++k) {
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }

  end = omp_get_wtime();
  printf("sequential time: %f sec\n", end - start);
}

int main() {
  init_matrix();
  parallel_multiply();
  opt_parallel_multiply();
  sequential_multiply();
}
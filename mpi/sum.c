#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  int rank, size, ierr;
  double sum = 0.0, result = 0.0;
  int n = atoi(argv[1]);

  ierr = MPI_Init(&argc, &argv);
  if (ierr != MPI_SUCCESS) {
    printf("MPI_Init failed\n");
    return 1;
  }

  ierr = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (ierr != MPI_SUCCESS) {
    printf("MPI_Comm_rank has error\n");
    return 1;
  }

  ierr = MPI_Comm_size(MPI_COMM_WORLD, &size);
  if (ierr != MPI_SUCCESS) {
    printf("MPI_Comm_size has error\n");
    return 1;
  }

  for (int i = rank + 1; i <= n; i += size) {
    sum += 1.0 / i;
  }

  MPI_Reduce(&sum, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0){
    printf("Sum: %f", result);
  }

  MPI_Finalize();
  return 0;
}
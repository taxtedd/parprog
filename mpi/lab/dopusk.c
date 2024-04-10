#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  int rank, size, ierr;

  ierr = MPI_Init(&argc, &argv);
  if (ierr != MPI_SUCCESS) {
    perror("MPI_Init failed\n");
    return 1;
  }

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    double message = 0;
    MPI_Send(&message, 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
  }

  if (rank == 1) {
    MPI_Status status;
    double message = 0;
    double start = MPI_Wtime();

    MPI_Recv(&message, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);

    double end = MPI_Wtime();
    double total_time = end - start;
    printf("sending time: %f s\n", total_time);
  }

  MPI_Finalize();

  return 0;
}
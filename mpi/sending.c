#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
  int rank, size, ierr;

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

  int message = 0;
  MPI_Status status;

  if (rank == 0) {
    message++;
    MPI_Send(&message, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

    MPI_Recv(&message, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD, &status);
    printf("rank:%d сообщение: %d\n", rank, message);
  } else {
    MPI_Recv(&message, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
    printf("rank:%d сообщение: %d\n", rank, message);

    message++;
    MPI_Send(&message, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}
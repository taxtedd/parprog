#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv){
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

  printf("Hello, world! rank:%d, size:%d\n", rank, size);
  MPI_Finalize();

  return 0;
}

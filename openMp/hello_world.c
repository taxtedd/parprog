#include <stdio.h>
#include <omp.h>

int main(int argc, char** argv){
#pragma omp parallel
  {
    printf("Hello, world! from process: %d\n", omp_get_thread_num());
  }
  return 0;
}
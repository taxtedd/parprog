#include <iostream>
#include <pthread.h>
#include <algorithm>

#define NUM_THREADS 4

// Размер массива
const long long array_size = 10000000;
long long array[array_size];

// Искомое число
long long target = 72;
// Индекс искомого числа
long long result = -1;

void *search(void *thread_id) {
  auto tid = (long long) thread_id;
  
  long long start = tid * (array_size / NUM_THREADS);
  long long end = (tid + 1) * (array_size / NUM_THREADS) - 1;
  if (tid == NUM_THREADS - 1) {
    end = array_size - 1;
  }

  for (long long i = start; i <= end; ++i) {
    if (array[i] == target) {
      result = i;
      break;
    }
  }

  pthread_exit(nullptr);
}

void parallel_search() {
  int rc;
  pthread_t threads[NUM_THREADS];

  for (long long i = 0; i < NUM_THREADS; ++i) {
    if ((rc = pthread_create(&threads[i], nullptr, search, (void*)i))) {
      std::cout << "Error: unable to create thread, " << rc << std::endl;
      exit(-1);
    }
  }

  for (long long i = 0; i < NUM_THREADS; ++i) {
    pthread_join(threads[i], nullptr);
  }

  if (result != -1) {
    std::cout << "Element was found at index: " << result << '\n';
  } else {
    std::cout << "Element was not found in the array\n";
  }
}

void one_thread_search() {
  bool found = false;

  for (long long i = 0; i < array_size; ++i) {
    if (array[i] == target) {
      found = true;
      std::cout << "Element was found in the array\n";
      break;
    }
  }

  if (!found) {
    std::cout << "Element was not found in the array\n";
  }
}

int main() {
  for (long long i = 0; i < array_size; ++i) {
    array[i] = 1 + std::rand() % 100;
  }

  clock_t start, end;

  start = clock();
  parallel_search();
  end = clock();
  std::cout <<"Parallel search time: " << (end - start)/ (double) CLOCKS_PER_SEC << " sec\n";

  start = clock();
  one_thread_search();
  end = clock();
  std::cout <<"One tread search time: " << (end - start)/ (double) CLOCKS_PER_SEC << " sec\n";
}
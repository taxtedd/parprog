#include <iostream>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 4

const int array_size = 10;
int array[array_size];
int array2[array_size];
int part = 0;
int border_l[NUM_THREADS];
int border_r[NUM_THREADS];

void merge(int left, int mid, int right) {
  int start = mid + 1;

  if (array[mid] <= array[start]) {
    return;
  }

  while (left <= mid && start <= right) {
    if (array[left] <= array[start]) {
      left++;
    } else {
      int value = array[start];
      int index = start;
      while (index != left) {
        array[index] = array[index - 1];
        index--;
      }
      array[left] = value;

      left++;
      mid++;
      start++;
    }
  }
}

void merge_sort(int left, int right) {
  if (left < right) {
    int mid = left + (right - left) / 2;

    merge_sort(left, mid);
    merge_sort(mid + 1, right);
    merge(left, mid, right);
  }
}

void* thread_merge_sort(void* arg) {
  int thread_index = part++;

  int left = border_l[thread_index];
  int right = border_r[thread_index];

  merge_sort(left, right);

  pthread_exit(NULL);
}

void print_time(clock_t start, clock_t end, const std::string& type) {
//  std::cout << "Sorted array: ";
//  for (int i = 0; i < array_size; ++i) {
//    std::cout << array[i] << " ";
//  }
//  std::cout << '\n';

  std::cout << type << " sort time: " << (end - start)/ (double) CLOCKS_PER_SEC << " sec\n";
}

void parallel_merge_sort() {
  clock_t start, end;
  start = clock();
  
  pthread_t threads[NUM_THREADS];
  for (int i = 0; i < NUM_THREADS; ++i) {
    border_l[i] = i * (array_size / NUM_THREADS);
    border_r[i] = (i + 1) * (array_size / NUM_THREADS) - 1;
    if (i == NUM_THREADS - 1) {
      border_r[i] = array_size - 1;
    }
    pthread_create(&threads[i], NULL, thread_merge_sort, (void *)NULL);
  }

  for (int i = 0; i < NUM_THREADS; i++)
    pthread_join(threads[i], NULL);

  merge(border_l[0], border_r[0], border_r[1]);
  merge(border_l[2], border_r[2], border_r[3]);
  merge(0, border_r[1], border_r[3]);

  end = clock();

  print_time(start, end, "parallel");
}

void one_thread_merge_sort() {
  clock_t start, end;

  start = clock();
  merge_sort(0, array_size - 1);
  end = clock();

  print_time(start, end, "one thread");
}

int main() {
  for (int i = 0; i < array_size; ++i) {
    array[i] = 1 + std::rand() % 100;
    array2[i] = array[i];
  }
//  std::cout << "Array: ";
//  for (int i = 0; i < array_size; ++i) {
//    std::cout << array[i] << " ";
//  }
//  std::cout<<'\n';

  parallel_merge_sort();
  for (int i = 0; i < array_size; ++i) {
    array[i] = array2[i];
  }

  one_thread_merge_sort();

  return 0;
}
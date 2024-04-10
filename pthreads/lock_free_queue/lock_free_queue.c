#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define NUM_THREADS 2
pthread_t threads[NUM_THREADS];

typedef struct node_s {
  struct node_s* next_ptr;
  int value;
} node_t;

typedef struct {
  node_t* head;
  node_t* tail;
  size_t length;
} queue_t;

queue_t* queue;
pthread_mutex_t mutex;

void queue_init(queue_t** queue) {
  *queue = (queue_t*)malloc(sizeof(queue_t));

  node_t* zero_node = (node_t*)malloc(sizeof(node_t));
  zero_node->value = -1;
  zero_node->next_ptr = NULL;

  (*queue)->head = zero_node;
  (*queue)->tail = zero_node;
  (*queue)->length = 0;
}

bool enqueue(int value) {
  node_t* tail;
  node_t* head;
  node_t* next_ptr;

  node_t* node = (node_t*)malloc(sizeof(node_t));
  node->value = value;
  node->next_ptr = NULL;

  node_t* tmp = NULL;

  while (true) {
    // save values
    tail = queue->tail;
    next_ptr = tail->next_ptr;

    // if tail is not an actual tail, fix and restart
    if (next_ptr != NULL) {
      __sync_bool_compare_and_swap(&queue->tail, tail, next_ptr);
      continue;
    }

    // add element
    if (__sync_bool_compare_and_swap(&tail->next_ptr, tmp, node)) {
      break;
    }
  }

  queue->length++;
  __sync_bool_compare_and_swap(&queue->tail, tail, node);
  free(tmp);

  return true;
}

node_t* dequeue() {
  node_t* head;
  node_t* tail;
  node_t* next_ptr;

  while (true) {
    // save values
    head = queue->head;
    next_ptr = head->next_ptr;

    // if head was changed by another thead, start from the beginning
    if (queue->head != head) {
      continue;
    }

    // if queue is empty
    if (next_ptr == NULL) {
      return NULL;
    }

    // if head has next, but tail==head, need to fix and restart
    tail = queue->tail;
    if (head == tail) {
      __sync_bool_compare_and_swap(&queue->tail, tail, next_ptr);
      continue;
    }

    //delete element
    if (__sync_bool_compare_and_swap(&queue->head->next_ptr, next_ptr, next_ptr->next_ptr)) {
      free(next_ptr);
      break;
    }
  }

  queue->length--;
  return next_ptr;
}

void queue_clean() {
  while (queue->head->next_ptr != NULL) {
    dequeue();
  }
  free(queue->head);
  free (queue);
}

void* thread_func(void* args) {
  for (int i = 0; i < 100000000; ++i) {
    enqueue(i);
  }

  for (int i = 0; i < 100; ++i) {
    dequeue();
  }

  pthread_exit(NULL);
}

void lock_free_queue() {
  queue_init(&queue);

  int rc;
  for (long long i = 0; i < NUM_THREADS; ++i) {
    if ((rc = pthread_create(&threads[i], NULL, thread_func, (void*)&i))) {
      printf("Error: unable to create thread, %d", rc);
      exit(-1);
    }
  }

  for (long long i = 0; i < NUM_THREADS; ++i) {
    pthread_join(threads[i], NULL);
  }

  queue_clean();
}

bool enqueue_standart(int value) {
  node_t* node = (node_t*)malloc(sizeof(node_t));
  node->value = value;
  node->next_ptr = NULL;

  pthread_mutex_lock(&mutex);
  // if queue is empty
  if (queue->head->next_ptr == NULL) {
    queue->head->next_ptr = node;
    queue->tail = node;
  } else {
    queue->tail->next_ptr = node;
    queue->tail = node;
  }
  queue->length++;
  pthread_mutex_unlock(&mutex);

  return true;
}

node_t* dequeue_standart() {
  pthread_mutex_lock(&mutex);

  // if queue is empty
  if (queue->head->next_ptr == NULL) {
    return NULL;
  }

  node_t* head = queue->head->next_ptr;
  node_t* next_head = head->next_ptr;
  queue->head->next_ptr = next_head;
  free(head);
  queue->length--;

  pthread_mutex_unlock(&mutex);

  return head;
}

void* thread_func_standart(void* args) {
  for (int i = 0; i < 100000000; ++i) {
    enqueue_standart(i);
  }

  for (int i = 0; i < 100; ++i) {
    dequeue_standart();
  }

  pthread_exit(NULL);
}

void queue_clean_standart() {
  while (queue->head->next_ptr != NULL) {
    dequeue_standart();
  }
  free(queue->head);
  free (queue);
}

void standart_queue() {
  queue_init(&queue);

  int rc;
  for (long long i = 0; i < NUM_THREADS; ++i) {
    if ((rc = pthread_create(&threads[i], NULL, thread_func_standart, (void*)&i))) {
      printf("Error: unable to create thread, %d", rc);
      exit(-1);
    }
  }

  for (long long i = 0; i < NUM_THREADS; ++i) {
    pthread_join(threads[i], NULL);
  }

  queue_clean_standart();
}

int main() {
  clock_t start, end;

  start = clock();
  lock_free_queue();
  end = clock();

  printf("Lock-free queue time: %f sec\n", (end - start)/ (double) CLOCKS_PER_SEC);

  start = clock();
  standart_queue();
  end = clock();

  printf("Standart queue time: %f sec\n", (end - start)/ (double) CLOCKS_PER_SEC);

  return 0;
}
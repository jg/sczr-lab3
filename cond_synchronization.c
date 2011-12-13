#include <stdio.h>
#include <pthread.h>

#define BUFFER_SIZE 10

#define PRODUCER_TIMEOUT 100000
#define CONSUMER_TIMEOUT 1000000
int buffer[BUFFER_SIZE];

// points to first free cell in buffer
int buffer_pointer;
// number of elements in buffer
int buffer_length;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_full = PTHREAD_COND_INITIALIZER;

void produce() {
  while (1) {
    int value = get_random_number();
    put(value);
    printf("Produced: %d\n", value);
    fflush(stdout);
    usleep(PRODUCER_TIMEOUT);
  }
}

void consume() {
  while (1) { 
    int value = get();
    printf("Consumed: %d\n", value);
    fflush(stdout);
    usleep(CONSUMER_TIMEOUT);
  }
}

void put(int value) {
  pthread_mutex_lock(&mutex);
  if ( !buffer_full_p() ) {
    buffer[(buffer_pointer + buffer_length) % BUFFER_SIZE] = value;
    buffer_length++;
  } else {
    printf("Buffer full going to sleep....!\n");
    fflush(stdout);
    // wait on buffer_full condition
    pthread_cond_wait(&buffer_full, &mutex);
  }
  pthread_mutex_unlock(&mutex);
}

int get() {
  pthread_mutex_lock(&mutex);
  if ( !buffer_empty_p() ) {
		int value;

    buffer_length--;
    pthread_mutex_unlock(&mutex);
    value = buffer[buffer_pointer++ % BUFFER_SIZE];
    // signal threads which wait because the buffer is full
		pthread_cond_signal(&buffer_full);
    return value;
  } else {
    printf("Buffer empty!\n");
    fflush(stdout);
  }
  pthread_mutex_unlock(&mutex);
}

int buffer_full_p() {
  return buffer_length == BUFFER_SIZE;
}

int buffer_empty_p() {
  return buffer_length == 0;
}

int get_random_number() {
  return rand() % 100;
}

int main() {
  pthread_t producer, consumer;

  int i = 0;
  for( i = 0; i < BUFFER_SIZE; ++i )
    buffer[i] = 0;
  buffer_pointer = 0;
  buffer_length  = 0;

  pthread_create(&producer, NULL, produce, (void*)NULL);
  pthread_create(&consumer, NULL, consume, (void*)NULL);

  pthread_join(producer, NULL);
  pthread_join(consumer, NULL);
}

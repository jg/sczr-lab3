#include <stdio.h>
#include <pthread.h>

#define BUFFER1_SIZE 10
#define BUFFER2_SIZE 10

#define PRODUCER_TIMEOUT 100000
#define CONSUMER_TIMEOUT 1000000
int buffer1[BUFFER1_SIZE];
int buffer2[BUFFER2_SIZE];

// points to first free cell in buffer
int buffer1_pointer;
// number of elements in buffer
int buffer1_length;

int buffer2_pointer;
int buffer2_length;

pthread_mutex_t buffer1_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t buffer2_mutex = PTHREAD_MUTEX_INITIALIZER;

void produce1() {
  while (1) {
    int value = get_random_number();

    pthread_mutex_lock(&buffer1_mutex);
    put1(value);
    pthread_mutex_unlock(&buffer1_mutex);

    printf("Produced: %d\n", value);
    fflush(stdout);
    usleep(PRODUCER_TIMEOUT);
  }
}

void produce2() {
  while (1) {
    int value = get_random_number();

    pthread_mutex_lock(&buffer2_mutex);
    put2(value);
    pthread_mutex_unlock(&buffer2_mutex);

    printf("Produced: %d\n", value);
    fflush(stdout);
    usleep(PRODUCER_TIMEOUT);
  }
}


void consume1() {
  while (1) { 
    int locked_buffer1 = pthread_mutex_trylock(&buffer1_mutex);
    int locked_buffer2 = pthread_mutex_trylock(&buffer2_mutex);
    if (locked_buffer1 && locked_buffer2) {
      int value = get1();
      printf("Consumed: %d\n", value);
      fflush(stdout);
      usleep(CONSUMER_TIMEOUT);

      pthread_mutex_unlock(&buffer1_mutex);
      pthread_mutex_unlock(&buffer2_mutex);
    } else {
    }
  }
}

void consume2() {
  while (1) { 
    int value = get2();
    printf("Consumed: %d\n", value);
    fflush(stdout);
    usleep(CONSUMER_TIMEOUT);
  }
}

void put1(int value) {
  if ( !buffer1_full_p() ) {
    buffer1[(buffer1_pointer + buffer1_length) % BUFFER1_SIZE] = value;
    buffer1_length++;
  } else {
    printf("Buffer full going to sleep....!\n");
    fflush(stdout);
  }
}

void put2(int value) {
  if ( !buffer2_full_p() ) {
    buffer2[(buffer2_pointer + buffer2_length) % BUFFER2_SIZE] = value;
    buffer2_length++;
  } else {
    printf("Buffer full going to sleep....!\n");
    fflush(stdout);
  }
}

int get1() {
  if ( !buffer1_empty_p() ) {
		int value;

    buffer1_length--;
    value = buffer1[buffer1_pointer++ % BUFFER1_SIZE];
    return value;
  } else {
    printf("Buffer empty!\n");
    fflush(stdout);
  }
}

int get2() {
  if ( !buffer2_empty_p() ) {
		int value;

    buffer2_length--;
    value = buffer2[buffer2_pointer++ % BUFFER2_SIZE];
    return value;
  } else {
    printf("Buffer empty!\n");
    fflush(stdout);
  }
}

int buffer1_full_p() {
  return buffer1_length == BUFFER1_SIZE;
}

int buffer1_empty_p() {
  return buffer1_length == 0;
}

int buffer2_full_p() {
  return buffer2_length == BUFFER2_SIZE;
}


int buffer2_empty_p() {
  return buffer2_length == 0;
}

int get_random_number() {
  return rand() % 100;
}

int main() {
  pthread_t producer1, producer2, consumer1, consumer2;

  int i = 0;
  for( i = 0; i < BUFFER1_SIZE; ++i ) buffer1[i] = 0;
  for( i = 0; i < BUFFER2_SIZE; ++i ) buffer2[i] = 0;
  
  buffer1_pointer = 0;
  buffer1_length  = 0;
  buffer2_pointer = 0;
  buffer2_length  = 0;

  pthread_create(&producer1, NULL, produce1, (void*)NULL);
  pthread_create(&producer2, NULL, produce2, (void*)NULL);
  pthread_create(&consumer1, NULL, consume1, (void*)NULL);
  pthread_create(&consumer2, NULL, consume2, (void*)NULL);

  pthread_join(producer1, NULL);
  pthread_join(consumer2, NULL);
  pthread_join(producer1, NULL);
  pthread_join(consumer2, NULL);
}

#include <stdio.h>
#include <pthread.h>

#define N 10
int buffer[N];

void produce() {
}

void consume() {
}

int get_random_number() {
  return rand() % 100;
}

int main() {
  pthread_t producer, consumer;

  pthread_create(&producer, NULL, produce, (void*)NULL);
  pthread_create(&consumer, NULL, produce, (void*)NULL);
}

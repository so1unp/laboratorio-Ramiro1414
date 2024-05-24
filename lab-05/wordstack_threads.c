#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "wordstack.h"

#define NAME_SHM "/ramiws"

int main(void) {

pthread_t producer_t, consumer_t

pthread_create(&producer_t, NULL, producer, params);
pthread_create(&consumer_t, NULL, consumer, params);


pthread_exit(NULL);
}
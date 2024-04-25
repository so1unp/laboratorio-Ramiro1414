#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_THREADS 1000

int sleepTime;

void *sleepThread(void *threadId) {
    long currentThreadId = (long) threadId;
    int threadSleepTime = (rand() % (sleepTime)) + 1;
    printf("Hilo %ld: dormire: %d\n", currentThreadId, threadSleepTime);
    sleep(threadSleepTime);
    pthread_exit((void *)(intptr_t) threadSleepTime);
}

int main(int argc, char* argv[])
{
    // Chequea los parametros
    if (argc != 3) {
        fprintf(stderr, "Uso: %s n t\n", argv[0]);
        fprintf(stderr, "\tn    Numero de hilos.\n");
        fprintf(stderr, "\tt    Tiempo de sleep de los hilos.\n");
        exit(EXIT_FAILURE);
    }

    long i = 1;

    pthread_t threads[MAX_THREADS]; // arreglo de hilos

    srand(getpid());

    int numThreads = atoi(argv[1]); // cantidad de hilos

    // Chequeo cantidad de hilos
    if (numThreads > MAX_THREADS) {
        fprintf(stderr, "El numero de hilos indicado supera el maximo.\nRecibido: %d\nMaximo: %d\n", numThreads, MAX_THREADS);
        exit(EXIT_FAILURE);
    }

    sleepTime = atoi(argv[2]); // tiempo de sleep de los hilos
    printf("cantidad de hilos: %d\n", numThreads);
    printf("tiempo de sleep de los hilos: %d\n", sleepTime);

    // crea los hilos
    for (i = 1; i <= numThreads; i++) {
        pthread_t thread;
        pthread_create(&thread, NULL, sleepThread, (void *) i);
        threads[i-1] = thread;
    }

    i = 0;

    // proceso padre espera a los hilos
    for (i = 0; i < numThreads; i++) {

        void* sleepingTime;
        pthread_join(threads[i], &sleepingTime);
        printf("Hilo %ld termino: %ld\n", i+1, (long) sleepingTime);
    }

    exit(EXIT_SUCCESS);
}
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Variable global que incrementan los hilos.
static long glob = 0;

void *increment_glob(void *loops)
{
    long numLoops = (long) loops;

    int loc, j;

    // incrementa glob
    for (j = 0; j < numLoops; j++) {
        loc = glob;
        loc++;
        glob = loc;
    }

    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    int loops;

    // Controla numero de argumentos.
    if (argc != 2) {
        fprintf(stderr, "Uso: %s ciclos\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    loops = atoi(argv[1]);

    // Verifica argumentos.
    if (loops < 1) {
        fprintf(stderr, "Error: ciclos debe ser mayor a cero.\n");
        exit(EXIT_FAILURE);
    }

    // crea hilos
    pthread_t thread1;
    pthread_t thread2;

    pthread_create(&thread1, NULL, increment_glob, (void *)(long) loops);
    pthread_create(&thread2, NULL, increment_glob, (void *)(long) loops);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("%ld\n", glob);

    exit(EXIT_SUCCESS);
}

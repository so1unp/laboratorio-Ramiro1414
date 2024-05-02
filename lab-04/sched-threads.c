#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <strings.h>
#include <pthread.h>
#include <sched.h>

pthread_t *threads;
int *buf;

int items;
int count;
int pos = 0;
int done = 0;

void *write_buffer(void *thread_id) {
    int i, j;
    long id;
    id = (long) thread_id;

    for (i = 0; i < items; i++) {   
        // Escribe el mismo valor en el mismo lugar varias veces.
        // Asi se consumo tiempo de CPU para ocupar lo más posible el quantum.
        for(j= 0; j < 0xFFFFFF; j++) { 
            buf[pos] = (int) id;
        }
        pos = pos + 1;
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int i, sched_policy;
    void *status;

    if (argc != 4) {
        fprintf(stderr, "Uso: %s [-R | -F] hilos items\n", argv[0]);
        fprintf(stderr, "\t-R       SCHED_RR.\n");
        fprintf(stderr, "\t-F       SCHED_FIFO.\n");
        fprintf(stderr, "\t-C       SCHED_OTHER.\n");
        fprintf(stderr, "\thilos    Número de hilos a generar.\n");
        fprintf(stderr, "\titems    Cuantos items escribe el hilo en el buffer.\n");
        exit(EXIT_FAILURE);
    }
    
    count = atoi(argv[2]);
    items = atoi(argv[3]);
    
    if (count <= 0) {
        fprintf(stderr, "Error: el numero de hilos debe ser mayor que cero.\n");
        exit(EXIT_FAILURE);
    }

    if (items <= 0) {
        fprintf(stderr, "Error: el numero de items debe ser mayor que cero.\n");
        exit(EXIT_FAILURE);
    }

    if (argv[1][1] != 'R' && argv[1][1] != 'F' && argv[1][1] != 'C') {
        fprintf(stderr, "Error: opción invalida %s.\nUsar -R (SCHED_RR) o -F (SCHED_FIFO)\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    if (argv[1][1] == 'C') {
        sched_policy = SCHED_OTHER;
    } 
    
    if (argv[1][1] == 'R') {
        sched_policy = SCHED_RR;
    }

    if (argv[1][1] == 'F') {
        sched_policy = SCHED_FIFO;
    }

    // Reserva espacio para un arreglo donde guardar variables de tipo pthread_t
    threads = (pthread_t*) malloc(count * sizeof(pthread_t));

    // Reserva espacio para el buffer donde los hilos escribiran su identificador
    buf = (int*) malloc( count * items * sizeof(int));
    bzero(buf, items * count * sizeof(int));

    srand(time(NULL));

    pthread_attr_t attr;
    struct sched_param param;

    // Inicializa la estructura attr
    pthread_attr_init(&attr);

    // Indica que al crear un hilo usando attr como parámetros, este debe
    // utilizar la política de planificación indicada en dichos parámetros.
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

    // Indica que la política de planificación será Round Robin. 
    pthread_attr_setschedpolicy(&attr, sched_policy);

    // Indica el nivel de prioridad que tendrá el hilo creado utilizando attr.
    param.sched_priority = 1;
 
    pthread_attr_setschedparam(&attr, &param);

    // Indica que el hilo creado utilizando el atributo attr debe ejecutar
    // siempre en la CPU 0.
    cpu_set_t cpuSet;
    CPU_ZERO (&cpuSet);
    CPU_SET (0, &cpuSet);
    pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuSet);

    // Crea los hilos.
    i = 0;
    for (i = 0; i < count; i++) {
        pthread_create(&threads[i], &attr, write_buffer, (void *)(intptr_t) i);
    }

    // Espera a que terminen todos los hilos.
    i = 0;
    for (i = 0; i < count; i++) {
        pthread_join(threads[i], &status);
    }

    // Imprime el buffer.
    for (i = 0; i < count * items; i++) {
        printf("%d ", buf[i]);
    }
    printf("\n");

    pthread_exit(NULL);
}

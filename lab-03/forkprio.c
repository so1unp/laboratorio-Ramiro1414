#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>      // times()
#include <unistd.h>
#include <signal.h>
#include <sys/resource.h>

#define MAX_CHILDS 100

int pids[MAX_CHILDS]; // arreglo que almacenara los pids de los procesos hijos
int childs = 0;

void terminateChild() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("Child %d (nice %2d):\t%3li\n", getpid(), getpriority(PRIO_PROCESS, (id_t) getpid()), usage.ru_utime.tv_sec);
    exit(0);
}

void doNothing() {
    return;
}

void terminateFather() {
    // printf("padre finaliza\n");
    int k = 1;
    for (k = 1; k <= childs; k++) {
        //printf("%d\n", pids[k-1]);
        kill(pids[k-1], SIGTERM); // termina la ejecución del hijo
    }
    // printf("hijos finalizados");
    exit(0);
}

int busywork(void)
{
    struct tms buf;
    for (;;) {
        times(&buf);
    }
}

int main(int argc, char *argv[])
{

    // controla que se ingresen solo tres argumentos
    if (argc != 4) {
        printf("Numero incorrecto de argumentos: %d (argumentos esperados: 3)", argc);
        exit(EXIT_FAILURE);
    }
    
    const int MAX_PRIORITY = 10;

    struct sigaction childHandler;
    childHandler.sa_handler = terminateChild;

    struct sigaction fatherHandler;
    fatherHandler.sa_handler = terminateFather;

    struct sigaction childHandlerDoNothing;
    childHandlerDoNothing.sa_handler = doNothing;

    // argumentos
    childs = atoi(argv[1]); // cantidad de hijos
    int execSeconds = atoi(argv[2]); // cantidad de segundos de ejecución
    int priorityReduction = atoi(argv[3]); // indica si se reducen las prioridades

    int i = 0;
    int j = 0;
    int p = getpid();
    int currentPriority = -1; // comienza en -1 porque se incrementa a 0 con el primer fork()

    // printf("soy el padre con pid: %d\n", p);

    // se crean los hijos
    for (i = 1; i <= childs; i++) {

        if (p > 0) {
            p = fork();
            pids[i-1] = p; // almaceno el pid del nuevo hijo
            if (priorityReduction == 1) {
                (currentPriority < MAX_PRIORITY) ? (currentPriority += 1) : (currentPriority = currentPriority); // reduce la prioridad del hijo
            } else {
                currentPriority = 0;
            }
        }

        if (p == 0) {
            // printf("soy el hijo %d con pid: %d\n", i, getpid());
            sigaction(SIGTERM, &childHandler, NULL); // asigna la señal de terminacion al hijo
            sigaction(SIGINT, &childHandlerDoNothing, NULL);
            nice(currentPriority);
            busywork(); // pone a ejecutar una tarea al hijo
        }
        
    }

    // finaliza la ejecución de los hijos
    if (p > 0) {

    // signal(SIGINT, notifyFather);
    sigaction(SIGINT, &fatherHandler, NULL); // asigna la señal de terminacion al hijo

        if ( execSeconds == 0) {
            while( pause() ) {
                // EJECUCION INDEFINIDA
            }
        } else {
            sleep( (unsigned int) execSeconds);

            // printf("padre finaliza\n");
            
            for (j = 1; j <= childs; j++) {
                kill(pids[j-1], SIGTERM); // termina la ejecución del hijo
            }
        }
    }

    exit(EXIT_SUCCESS);
}
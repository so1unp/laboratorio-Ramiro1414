#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>      // times()
#include <unistd.h>
#include <signal.h>
#include <sys/resource.h>

void terminateChild(int signum) {
    printf("SIGNAL [%d]: hijo con pid: %d y prioridad: %d\n", signum, getpid(), getpriority(PRIO_PROCESS, (id_t) getpid()));
}

void childLoop(void) {
    while (1) {
        // loop infinito que no hace nada
    }
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

    signal(SIGTERM, terminateChild); // asigna la señal de terminacion al hijo

    // argumentos
    int childs = atoi(argv[1]); // cantidad de hijos
    int execSeconds = atoi(argv[2]); // cantidad de segundos de ejecución
    int priorityReduction = atoi(argv[3]); // indica si se reducen las prioridades

    int i = 0;
    int j = 0;
    int p = getpid();
    int pids[childs]; // arreglo que almacenara los pids de los procesos hijos
    int currentPriority = -1; // comienza en -1 porque se incrementa a 0 con el primer fork()

    printf("soy el padre con pid: %d\n", p);

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
            printf("soy el hijo %d con pid: %d\n", i, getpid());
            nice(currentPriority);
            childLoop(); // pone a ejecutar una tarea al hijo
            // break;
        }
        
    }

    // finaliza la ejecución de los hijos
    if (p > 0) {

        sleep( (unsigned int) execSeconds);

        printf("padre finaliza\n");
        
        for (j = 1; j <= childs; j++) {
            kill(pids[j-1], SIGTERM); // termina la ejecución del hijo
        }
        
    }

    exit(EXIT_SUCCESS);
}
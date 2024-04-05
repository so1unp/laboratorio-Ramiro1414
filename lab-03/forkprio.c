#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>      // times()
#include <unistd.h>
#include <signal.h>
#include <sys/resource.h>

void terminateChild(int signum) {
    printf("hijo terminado por señal: %d", signum);
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

    /* ========================= DESCOMENTAR LUEGO =========================
    // controla que se ingresen solo tres argumentos
    if (argc != 3) {
        printf("Numero incorrecto de argumentos (argumentos esperados: 3)");
        exit(EXIT_FAILURE);
    }
    */

    // argumentos
    int childs = atoi(argv[1]); // cantidad de hijos
    int execSeconds = atoi(argv[2]); // cantidad de segundos de ejecución
    // int priorityReduction = atoi(argv[3]); // indica si se reducen las prioridades

    int i = 0;
    int j = 0;
    int p = getpid();
    int pids[childs]; // arreglo que almacenara los pids de los procesos hijos

    printf("soy el padre con pid: %d\n", p);

    // se crean los hijos
    for (i = 1; i <= childs; i++) {

        if (p > 0) {
            p = fork();
            pids[i-1] = p; // almaceno el pid del nuevo hijo
        }

        if (p == 0) {
            printf("soy el hijo %d con pid: %d\n", i, getpid());
            signal(SIGTERM, terminateChild); // asigna la señal de terminacion al hijo
            busywork(); // pone a ejecutar una tarea al hijo
            break;
        }
        
    }

    // finaliza la ejecución de los hijos
    if (p > 0) {

        sleep( (unsigned int) execSeconds);
        printf("padre finaliza\n");

        for (j = 1; j <= childs; j++) {
            printf("hijo %d con pid: %d\n", j, pids[j-1]);
            kill(pids[j-1], SIGTERM); // termina la ejecución del hijo
        }

    }

    exit(EXIT_SUCCESS);
}
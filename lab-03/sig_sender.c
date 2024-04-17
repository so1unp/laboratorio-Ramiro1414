#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char *argv[]) {

    if(argc != 3) {
        printf("Numero incorrecto de argumentos. Uso: sig_sender <processId> <signalNumber>");
        exit(EXIT_FAILURE);
    }

    int processId = atoi(argv[1]);
    int signalNumber = atoi(argv[2]);

    kill(processId, signalNumber);

    exit(EXIT_SUCCESS);
}
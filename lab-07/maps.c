#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <strings.h>

int a = 0;
char *p;

void handler(int p)
{
    return;
}

int main(void) {
    signal(SIGUSR1, handler);
    p = (char*) malloc(sizeof(char) * 8192);

    pid_t pid = fork();

    // Esperamos la señal SIGUSR1
    pause();

    if (pid == 0) {
        // Agregar aquí el código pedido en el ejercicio.

        // Esperamos la señal SIGUSR1
        pause();
    } else {
        // Esperamos la señal SIGUSR1
        pause();
    }

    return 0;
}

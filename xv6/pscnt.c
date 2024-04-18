/*
 * Invoca la llamada al sistema getppid().
 */
#include "types.h"
#include "user.h"
#include "date.h"

int main(int argc, char *argv[])
{
    // Descomentar una vez implementada la llamada al sistema.
    printf(1, "Procesos antes de hacer fork: [%d]\n", pscnt());

    int p = fork();
    
    if (p != 0) {
        printf(1, "Procesos luego de hacer fork: [%d]\n", pscnt());
    }

    exit();
}
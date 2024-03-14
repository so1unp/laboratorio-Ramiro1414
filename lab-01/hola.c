#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Agregar código aquí.
    int i;

    for (i=1; i<argc; i++) {
    	printf("%s ", argv[i]);
    }
    
    printf("\n");

    // Termina la ejecución del programa.
    exit(EXIT_SUCCESS);
}

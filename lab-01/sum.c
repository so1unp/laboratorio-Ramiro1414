#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Agregar código aquí.
	
	int i = 0;
	int sum = 0;

	for(i=1; i<argc; i++){
		sum += atoi(argv[i]);
	}
	
	printf("%d\n", sum);
	
    // Termina la ejecución del programa.
    exit(EXIT_SUCCESS);
}

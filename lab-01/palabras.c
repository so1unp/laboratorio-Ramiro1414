#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Agregar código aquí.
	
	int c;
	c = getchar();

	while (1) {
	
	if (c == ' ')
		printf("\n");
	else
		putchar(c);	

	c = getchar();

	}	

	printf("\n");

    // Termina la ejecución del programa.
    exit(EXIT_SUCCESS);
}

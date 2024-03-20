#include <stdio.h>
#include <stdlib.h>

#define MAX_WORD_LENGTH 5

int main(int argc, char *argv[])
{
    // Agregar código aquí.
	
	int c, i, j, k;
	c = getchar();
	i = 0;
	j = 0;
	k = 0;
	int letters;
	letters = 0;
	int lettersPerWord[MAX_WORD_LENGTH];
	
	for (i = 0; i < MAX_WORD_LENGTH; i++) {
		lettersPerWord[i] = 0;
	}
	
	// Count words length
	while (c != EOF) {
	
	if (c == ' ' || c == '\n') {
		if (letters <= MAX_WORD_LENGTH) {
			lettersPerWord[letters-1] = lettersPerWord[letters-1] + 1;
			letters = 0;
		}
	}
	else {
		letters++;
	}

	c = getchar();
	
	}	

	printf("\n");
	
	// Print histogram
	for (j = 0; j < MAX_WORD_LENGTH; j++) {
		printf("%d ", j+1);
		
		for (k = 0; k < lettersPerWord[j]; k++) {
		printf("*");
		}
		
		printf("\n");
	}

    // Termina la ejecución del programa.
    exit(EXIT_SUCCESS);
}

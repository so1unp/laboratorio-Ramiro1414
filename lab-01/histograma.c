#include <stdio.h>
#include <stdlib.h>

#define MAX_WORD_LENGTH 26

int main(int argc, char *argv[])
{
    // Agregar código aquí.
	
	int c, i, j, k, l, longestWord;
	c = getchar();
	i = 0;
	j = 0;
	k = 0;
	l = 0;
	longestWord = 0;
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
			lettersPerWord[letters] = lettersPerWord[letters] + 1;
			letters = 0;
		}
		else {
			letters = 0;
		}
	}
	else {
		letters++;
	}

	c = getchar();
	
	}	

	printf("\n");
	
	// Find longest word
	for (l = 1; l < MAX_WORD_LENGTH; l++) {
		if (lettersPerWord[l] != 0) {
			longestWord = l;
		}
	}

	// Print histogram
	for (j = 1; j <= longestWord; j++) {
		printf("%d ", j);
		
		for (k = 0; k < lettersPerWord[j]; k++) {
		printf("*");
		}
		
		printf("\n");
	}

    // Termina la ejecución del programa.
    exit(EXIT_SUCCESS);
}

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define ITEMS       3
#define MAX_WORD    50
#define SHM_PERMISSIONS 0666

struct wordstack {
    int free;
    int items;
    int max_word;
    pthread_mutex_t mutex;
    sem_t full;
    sem_t empty;
    char heap[ITEMS][MAX_WORD];
};

typedef struct wordstack wordstack_t;

void usage(char *argv[])
{
    fprintf(stderr, "Uso: %s comando parametro\n", argv[0]);
    fprintf(stderr, "Comandos:\n");
    fprintf(stderr, "\t-w pila palabra: agrega palabra en la pila\n");
    fprintf(stderr, "\t-r pila pos: elimina la palabra de la pila.\n");
    fprintf(stderr, "\t-p pila: imprime la pila de palabras.\n");
    fprintf(stderr, "\t-c pila: crea una zona de memoria compartida con el nombre indicado donde almacena la pila.\n");
    fprintf(stderr, "\t-d pila: elimina la pila indicada.\n");
    fprintf(stderr, "\t-h imprime este mensaje.\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        usage(argv);
        exit(EXIT_FAILURE);
    }

    if (argv[1][0] != '-') {
        usage(argv);
        exit(EXIT_FAILURE);
    }

    char option = argv[1][1];
    
    wordstack_t* wordstackInProcessMemory = NULL;

    int shm_fd; // descriptor de segmento de memoria compartida

    switch(option) {
        case 'w':

            /* PUSH */
            /*
                argv[2]: nombre de la memoria compartida
                argv[3]: palabra a escribir en la pila
            */

            if ((shm_fd = shm_open(argv[2], O_RDWR, SHM_PERMISSIONS)) == -1) {
                fprintf(stderr, "Error al abrir segmento de memoria compartida.\n");
                exit(EXIT_FAILURE);
            }

            wordstackInProcessMemory = (wordstack_t*) mmap(NULL, sizeof(wordstack_t), PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);

            if (wordstackInProcessMemory == MAP_FAILED) {
                fprintf(stderr, "Error al hacer mmap.\n");
                exit(EXIT_FAILURE);
            } else {

                int i = 0;
                sem_wait(&(wordstackInProcessMemory->empty));
                pthread_mutex_lock(&(wordstackInProcessMemory->mutex));
                for (i = 0; i < strlen(argv[3]); i++) {
                    wordstackInProcessMemory->heap[wordstackInProcessMemory->free][i] = argv[3][i];
                }

                wordstackInProcessMemory->items++;
                wordstackInProcessMemory->free++;
                pthread_mutex_unlock(&(wordstackInProcessMemory->mutex));
                sem_post(&(wordstackInProcessMemory->full));

                printf("Palabra %s agregada a la pila %s.\n", argv[3], argv[2]);

            }

            close(shm_fd);

            break;
        case 'r':

            /* POP */

            shm_fd = shm_open(argv[2], O_RDWR, SHM_PERMISSIONS);
            wordstackInProcessMemory = (wordstack_t*) mmap(NULL, sizeof(wordstack_t), PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);

            // posicion en la pila de la palabra que se eliminara
            // como free apunta al lugar libre, se quiere la direccion anterior a free
            int wordIndex = (wordstackInProcessMemory->free) - 1;

            // codigo para controlar el indice de la palabra que se eliminara
            if  (wordIndex == -1) {
                wordIndex = 0;
            }

            char word[MAX_WORD];

            int k = 0;
            sem_wait(&(wordstackInProcessMemory->full));
            pthread_mutex_lock(&(wordstackInProcessMemory->mutex));
            for (k = 0; k < MAX_WORD; k++) {
                word[k] = wordstackInProcessMemory->heap[wordIndex][k];
                wordstackInProcessMemory->heap[wordIndex][k] = ' ';
            }

            word[MAX_WORD] = '\0';

            wordstackInProcessMemory->items--;
            wordstackInProcessMemory->free--;
            pthread_mutex_unlock(&(wordstackInProcessMemory->mutex));
            sem_post(&(wordstackInProcessMemory->empty));

            printf("Palabra eliminada de la pila %s. La palabra era: %s\n", argv[2], word);

            break;
        case 'p':

            /* IMPRIMIR LA PILA */

            shm_fd = shm_open(argv[2], O_RDWR, SHM_PERMISSIONS);
            wordstackInProcessMemory = (wordstack_t*) mmap(NULL, sizeof(wordstack_t), PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);

            if (wordstackInProcessMemory->items == 0) {
                printf("Pila vacia.\n");
                exit(EXIT_SUCCESS);
            }

            int i = 0;
            int j = 0;
            
            for (i = wordstackInProcessMemory->free; i >= 0; i--) {
            
                for (j = 0; j < MAX_WORD; j++) {
                    if (wordstackInProcessMemory->heap[i][j] == ' ') {
                        putchar(' ');
                    } else {
                        putchar(wordstackInProcessMemory->heap[i][j]);
                    }
                }
                printf("\n");
            }
            printf("\n");

            close(shm_fd);

            break;
        case 'c':

            umask(0000);
            shm_fd = shm_open(argv[2], O_CREAT | O_RDWR, SHM_PERMISSIONS);
            if (shm_fd == -1) {
                fprintf(stderr, "Error al crear segmento de memoria compartida.\n");
                exit(EXIT_FAILURE);
            }

            if (ftruncate(shm_fd, sizeof(wordstack_t)) == -1) {
                fprintf(stderr, "Error al asignar tamaño del segmento de memoria compartida.\n");
            }

            wordstackInProcessMemory = (wordstack_t*) mmap(NULL, sizeof(wordstack_t), PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);

            pthread_mutex_init(&(wordstackInProcessMemory->mutex), NULL);
            sem_init(&(wordstackInProcessMemory->empty), 1, ITEMS);
            sem_init(&(wordstackInProcessMemory->full), 1, 0);

            wordstackInProcessMemory->items = 0;
            wordstackInProcessMemory->free = 0;

            printf("Memoria compartida %s creada con exito.\n", argv[2]);

            close(shm_fd);

            break;
        case 'd':

            shm_fd = shm_open(argv[2], O_RDWR, SHM_PERMISSIONS);
            wordstackInProcessMemory = (wordstack_t*) mmap(NULL, sizeof(wordstack_t), PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);

            sem_destroy(&(wordstackInProcessMemory->empty));
            sem_destroy(&(wordstackInProcessMemory->full));
            pthread_mutex_destroy(&(wordstackInProcessMemory->mutex));

            close(shm_fd);

            if (shm_unlink(argv[2]) == -1) {
                fprintf(stderr, "Error al eliminar el segmento de memoria compartida.\n");
                exit(EXIT_FAILURE);
            }


            printf("Memoria compartida %s borrada con exito.\n", argv[2]);

            break;
        case 'h':
            usage(argv);
            break;
        default:
            fprintf(stderr, "-%c: opción desconocida.\n", option);
            exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}

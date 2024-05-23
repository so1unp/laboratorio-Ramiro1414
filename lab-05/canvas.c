#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define HEIGHT  25  // Altura en caracteres de la pizarra
#define WIDTH   25  // Ancho en caracteres de la pizarra
#define SHM_PERMISSIONS 0622

struct canvas {
    char canvas[HEIGHT*WIDTH];
};

typedef struct canvas canvas_t;

void usage(char *argv[])
{
    fprintf(stderr, "Uso: %s comando parametro\n", argv[0]);
    fprintf(stderr, "Comandos:\n");
    fprintf(stderr, "\t-w canvas mensaje x y [h|v]: escribe el mensaje en el canvas indicado en la posición (x,y) de manera [h]orizontal o [v]ertical.\n");
    fprintf(stderr, "\t-p canvas: imprime el canvas indicado.\n");
    fprintf(stderr, "\t-c canvas: crea un canvas con el nombre indicado.\n");
    fprintf(stderr, "\t-d canvas: elimina el canvas indicado.\n");
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
    int x = 0;
    int y = 0;
    int shm_fd;
    canvas_t myCanvas;
    canvas_t* canvasInProcessMemory = NULL;

    switch(option) {
        case 'w':
            printf("Escribe %s en el canvas %s en la posición (%d, %d).\n", argv[3], argv[2], atoi(argv[4]), atoi(argv[5]));

            if ((shm_fd = shm_open(argv[2], O_RDWR, SHM_PERMISSIONS)) == -1) {
                fprintf(stderr, "Error al abrir segmento de memoria compartida.\n");
                exit(EXIT_FAILURE);
            }

            canvasInProcessMemory = (canvas_t*) mmap(NULL, sizeof(canvas_t), PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);

            if (canvasInProcessMemory == MAP_FAILED) {
                fprintf(stderr, "Error al hacer mmap.\n");
                exit(EXIT_FAILURE);
            } else {
            x = atoi(argv[4]);
            y = atoi(argv[5]);
            // strncpy( &(canvasInProcessMemory->canvas[(HEIGHT * x) + y]) , argv[3], strlen(argv[3]));

            int k = 0;
            for (k = 0; k < strlen(argv[3]); k++) {
                canvasInProcessMemory->canvas[((HEIGHT * x) + y + k) % (HEIGHT*WIDTH)] = argv[3][k];
            }

            }

            close(shm_fd);

            break;
        case 'p':
            printf("Imprime canvas.\n");

            shm_fd = shm_open(argv[2], O_RDWR, SHM_PERMISSIONS);

            canvasInProcessMemory = mmap(NULL, sizeof(myCanvas), PROT_WRITE, MAP_SHARED, shm_fd, 0);

            int i = 0;
            int j = 0;
            for (i = 0; i < HEIGHT; i++) {

                for (j = 0; j < WIDTH; j++) {
                    if (canvasInProcessMemory->canvas[(HEIGHT * i) + j] == '\0') { // espacio en blanco
                        putchar(' ');
                    } else {
                        putchar(canvasInProcessMemory->canvas[(HEIGHT * i) + j]);
                    }
                }
                printf("\n"); // nueva linea
            }
            printf("\n");

            close(shm_fd);

            break;
        case 'c':
            printf("Crea canvas %s.\n", argv[2]);

            // creo descriptor del segmento de memoria compartida
            umask(0000);
            shm_fd = shm_open(argv[2], O_CREAT | O_RDWR, SHM_PERMISSIONS);
            if (shm_fd == -1) {
                fprintf(stderr, "Error al crear segmento de memoria compartida.\n");
                exit(EXIT_FAILURE);
            }

            // asigno el tamaño del segmento de memoria compartida
            if (ftruncate(shm_fd, sizeof(canvas_t)) == -1) {
                fprintf(stderr, "Error al asignar tamaño del segmento de memoria compartida.\n");
            }

            close(shm_fd);

            break;
        case 'd':
            printf("Borra canvas %s.\n", argv[2]);

            // elimino el segmento de memoria compartida
            if (shm_unlink(argv[2]) == -1) {
                fprintf(stderr, "Error al eliminar el segmento de memoria compartida.\n");
                exit(EXIT_FAILURE);
            }

            break;
        case 'h':
            usage(argv);
            break;
        default:
            fprintf(stderr, "Comando desconocido\n");
            exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

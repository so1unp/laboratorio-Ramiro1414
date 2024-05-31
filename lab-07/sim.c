#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESOS 100
#define MEMORIA_VIRTUAL 64
#define MEMORIA_FISICA 32
#define TAMANIO_PAGINA 4
#define ALMACENAMIENTO_SECUNDARIO 64

struct process {
    int pid;
    int page_table[MEMORIA_VIRTUAL / TAMANIO_PAGINA];
    int busy;
};

typedef struct process process_t;

struct frame {
    int pid;
    int page;
};

typedef struct frame frame_t;

int main(int argc, char* argv[])
{

    if (argc != 2) {
        fprintf(stderr, "Numero incorrecto de parametros.\n");
        exit(EXIT_FAILURE);
    }

    if (argv[1][0] != '-') {
        fprintf(stderr, "Debe incluir '-' seguido de [f|l].\n");
        exit(EXIT_FAILURE);
    }

    char opcion = argv[1][1];

    int pid;
    int pagina;
    int memoria = 1;
    process_t procesos[MAX_PROCESOS];
    frame_t memoriaFisica[MEMORIA_FISICA / TAMANIO_PAGINA]; // Memoria fisica, es una coleccion de frames (marcos)

    int i = 0;
    int j = 0;
    int k = 0;

    // inicializo el arreglo de procesos
    for (i = 0; i < (MAX_PROCESOS); i++) {
        procesos[i].pid = -1;
        procesos[i].busy = 0;
    }

    // inicializo la memoria vacia
    for (i = 0; i < (MEMORIA_FISICA / TAMANIO_PAGINA); i++) {
        memoriaFisica[i].pid = -1;
        memoriaFisica[i].page = -1;
    }

    switch (opcion) {
        case 'f': 
        
            while (scanf("%d\n%d", &pid, &pagina) != EOF) {

                if (procesos[pid-1].busy == 0 && procesos[pid-1].pid == -1) { // si no existe el proceso, creo uno nuevo

                    // creo el proceso
                    process_t nuevoProceso;

                    // cargo con -1 la entrada de la tabla de paginas de ese proceso
                    for (i = 0; i < (MEMORIA_VIRTUAL / TAMANIO_PAGINA); i++) {
                        nuevoProceso.page_table[i] = -1;
                    }

                    nuevoProceso.pid = pid;
                    nuevoProceso.busy = 1;

                    // accedo a una pagina
                    nuevoProceso.page_table[pagina-1] = memoria;
                
                    // guardo al proceso en la tabla de procesos
                    procesos[pid-1] = nuevoProceso;

                    // cargo pagina en memoria fisica
                    frame_t nuevoMarco;
                    nuevoMarco.pid = pid;
                    nuevoMarco.page = pagina;

                    memoriaFisica[memoria-1] = nuevoMarco;
                    memoria++;
                } else { // si existe el proceso, agrego el acceso a pagina

                    if (procesos[pid-1].page_table[pagina-1] == -1) { // verifico que el proceso no quiera acceder a la misma pagina
                        procesos[pid-1].page_table[pagina-1] = memoria;

                        frame_t nuevoMarco;
                        nuevoMarco.pid = pid;
                        nuevoMarco.page = pagina;

                        memoriaFisica[memoria-1] = nuevoMarco;
                        memoria++;
                    }
                }
            }

            break;
        
        case 'l':

            // NO IMPLEMENTADO

            break;
        default:
            fprintf(stderr, "Argumento invalido. Usar [f|l]\n");
            exit(EXIT_FAILURE);   
    }

    // Imprimo la tabla de paginacion de cada proceso
    
    for (j = 0; j < MAX_PROCESOS; j++) {

        if (procesos[j].busy == 1 && procesos[j].pid != -1) {
            printf("Proceso %d: ", procesos[j].pid);

            for (k = 0; k < (MEMORIA_VIRTUAL / TAMANIO_PAGINA); k++) {
                if (procesos[j].page_table[k] == -1) {
                    printf("- ");
                } else {
                    printf("%d ", procesos[j].page_table[k]);
                }
            }
            printf("\n");
        }
    }

    // Imprimo la memoria fisica
    j = 0;
    printf("Memoria fisica: ");
    for (j = 0; j < (MEMORIA_FISICA / TAMANIO_PAGINA); j++) {

        if (memoriaFisica[j].pid == -1 && memoriaFisica[j].page == -1) {
            printf("- ");
        } else {
            printf("%d.%d ", memoriaFisica[j].pid, memoriaFisica[j].page);
        }
    }
    printf("\n");

    exit(EXIT_SUCCESS);
}
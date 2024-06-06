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
    int hayHueco;
    int indiceRAM = 0;
    int indiceDISCO = 0;
    process_t procesos[MAX_PROCESOS];
    frame_t memoriaFisica[MEMORIA_FISICA / TAMANIO_PAGINA]; // Memoria fisica, es una coleccion de frames (marcos)
    frame_t memoriaSecundaria[ALMACENAMIENTO_SECUNDARIO / TAMANIO_PAGINA];

    int i = 0;
    int j = 0;
    int k = 0;

    // inicializo el arreglo de procesos
    for (i = 0; i < (MAX_PROCESOS); i++) {
        procesos[i].pid = -1;
    }

    // inicializo la memoria fisica vacia
    for (i = 0; i < (MEMORIA_FISICA / TAMANIO_PAGINA); i++) {
        memoriaFisica[i].pid = -1;
        memoriaFisica[i].page = -1;
    }

    // inicializo la memoria secundaria
    for (i = 0; i < (ALMACENAMIENTO_SECUNDARIO / TAMANIO_PAGINA); i++) {
        memoriaSecundaria[i].pid = -1;
        memoriaSecundaria[i].page = -1;
    }

    switch (opcion) {
        case 'f': 
        
            while (scanf("%d\n%d", &pid, &pagina) != EOF) {

                hayHueco = 0;

                process_t proceso;

                if (procesos[pid-1].pid == -1) { // si no existe el proceso, creo uno nuevo
                    proceso.pid = pid;
                    proceso.page_table[pagina-1] = pagina;

                    // cargo con -1 la entrada de la tabla de paginas de ese proceso
                    for (i = 0; i < (MEMORIA_VIRTUAL / TAMANIO_PAGINA); i++) {
                        proceso.page_table[i] = -1;
                    }

                } else { // si existe, lo recupero
                    proceso = procesos[pid-1];
                }

                // busco lugar disponible en memoria fisica
                if(memoriaFisica[indiceRAM].pid == -1 && memoriaFisica[indiceRAM].page == -1) {

                    // accedo a una pagina
                    proceso.page_table[pagina-1] = indiceRAM+1;

                    // guardo al proceso en la tabla de procesos
                    procesos[pid-1] = proceso;

                    // cargo pagina en memoria fisica
                    frame_t nuevoMarco;
                    nuevoMarco.pid = pid;
                    nuevoMarco.page = pagina;

                    memoriaFisica[indiceRAM] = nuevoMarco;
                    indiceRAM = (indiceRAM + 1) % (MEMORIA_VIRTUAL / TAMANIO_PAGINA);
                    hayHueco = 1;
                }
                
                if (!hayHueco) { // si no hay lugar en la memoria fisica, hago swap                

                    proceso.page_table[pagina-1] = indiceDISCO+1;
                    procesos[pid-1] = proceso;

                    // actualizo el indice de ram
                    indiceRAM = (indiceRAM + 1) % (MEMORIA_FISICA / TAMANIO_PAGINA);
                    
                    // bajo a disco el proceso
                    frame_t nuevaPaginaDisco;
                    nuevaPaginaDisco.pid = memoriaFisica[indiceRAM-1].pid;
                    nuevaPaginaDisco.page = memoriaFisica[indiceRAM-1].page;
                    memoriaSecundaria[indiceDISCO] = nuevaPaginaDisco;

                    // actualizo el indice de disco
                    indiceDISCO = (indiceDISCO + 1) % (ALMACENAMIENTO_SECUNDARIO / TAMANIO_PAGINA);

                    // meto en memoria el nuevo proceso
                    frame_t nuevoMarco;
                    nuevoMarco.pid = pid;
                    nuevoMarco.page = pagina;

                    memoriaFisica[indiceRAM-1] = nuevoMarco;

                    hayHueco = 0;
                }

            } // end while

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

        if (procesos[j].pid != -1) {
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

    // Imprimo la memoria secundaria
    j = 0;
    printf("Memoria secundaria: ");
    for (j = 0; j < (ALMACENAMIENTO_SECUNDARIO / TAMANIO_PAGINA); j++) {

        if (memoriaSecundaria[j].pid == -1 && memoriaSecundaria[j].page == -1) {
            printf("- ");
        } else {
            printf("%d.%d ", memoriaSecundaria[j].pid, memoriaSecundaria[j].page);
        }
    }
    printf("\n");

    exit(EXIT_SUCCESS);
}
#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESOS 100
#define MEMORIA_VIRTUAL 64
#define MEMORIA_FISICA 32
#define TAMANIO_PAGINA 4
#define ALMACENAMIENTO_SECUNDARIO 64

struct proceso {
    int pid;
    int memoriaVirtual[MEMORIA_VIRTUAL / TAMANIO_PAGINA];
    int ocupado;
};

typedef struct proceso proceso_t;

struct marco {
    int pid;
    int pagina;
};

typedef struct marco marco_t;

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

    /* --------------------- DESCOMENTAR LUEGO ---------------------
    int memoriaFisica[MEMORIA_FISICA / TAMANIO_PAGINA];
    int almacenamientoSecundario[ALMACENAMIENTO_SECUNDARIO / TAMANIO_PAGINA];
    */ 

    int pid;
    int pagina;
    proceso_t procesos[MAX_PROCESOS];
    marco_t marcos[MEMORIA_FISICA / TAMANIO_PAGINA]; // Memoria fisica, es una coleccion de frames (marcos)

    switch (opcion) {
        case 'f': 
        
            while (pid != EOF && pagina != EOF) {

                // creo el proceso
                scanf("%d", &pid);
                proceso_t nuevoProceso;
                nuevoProceso.pid = pid;
                nuevoProceso.ocupado = 1;

                // accedo a una pagina
                scanf("%d", &pagina);
                nuevoProceso.memoriaVirtual[pagina-1] = pagina;

                procesos[pid-1] = nuevoProceso;

                marco_t nuevoMarco;
                nuevoMarco.pid = pid;
                nuevoMarco.pagina = pagina;

                printf("%d.%d\n", nuevoMarco.pid, nuevoMarco.pagina);
            }

            break;
        
        case 'l':

            // NO IMPLEMENTADO

            break;
        default:
            fprintf(stderr, "Argumento invalido. Usar [f|l]\n");
            exit(EXIT_FAILURE);   
    }

    printf("pid: %d\n", procesos[0].pid);
    printf("pid: %d\n", marcos[0].pid);
    exit(EXIT_SUCCESS);
}

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_PROCESOS 100
#define MEMORIA_VIRTUAL 64
#define MEMORIA_FISICA 32
#define TAMANIO_PAGINA 4
#define ALMACENAMIENTO_SECUNDARIO 64

struct pagina {
    int pid;
    int pagina;
};

typedef struct pagina pagina_t;

struct proceso {
    int pid;
    int tabla_paginas[MEMORIA_VIRTUAL / TAMANIO_PAGINA];
};

typedef struct proceso proceso_t;

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
    int indiceRAM = 0;
    int indiceRAM_LRU = 0;
    int indiceDISCO = 0;
    int indicePaginaEnDisco = 0;
    int min = 0;
    int max = 0;
    int indiceMin = 0;
    int indiceProcesoPaginaEnRAM = 0;

    bool procesoPaginaEnRAM = false;
    bool procesoPaginaEnDISCO = false;
    bool hayLugarEnRAM = false;

    proceso_t procesos[MAX_PROCESOS];
    pagina_t memoriaFisica[MEMORIA_FISICA / TAMANIO_PAGINA]; // Memoria fisica, es una coleccion de frames (marcos)
    pagina_t memoriaSecundaria[ALMACENAMIENTO_SECUNDARIO / TAMANIO_PAGINA]; // Memoria secundaria, es una coleccion de paginas

    int tiempo = 0;
    int tiempoRAM[MEMORIA_FISICA / TAMANIO_PAGINA];

    int i = 0;
    int j = 0;
    int k = 0;
    
    // inicializo arreglo de tiempo para LRU
    for (i = 0; i < (MEMORIA_FISICA / TAMANIO_PAGINA); i++) {
        tiempoRAM[i] = 0;
    }

    // inicializo el arreglo de procesos
    for (i = 0; i < (MAX_PROCESOS); i++) {
        procesos[i].pid = -1;
    }

    // inicializo la memoria fisica vacia
    for (i = 0; i < (MEMORIA_FISICA / TAMANIO_PAGINA); i++) {
        memoriaFisica[i].pid = -1;
        memoriaFisica[i].pagina = -1;
    }

    // inicializo la memoria secundaria
    for (i = 0; i < (ALMACENAMIENTO_SECUNDARIO / TAMANIO_PAGINA); i++) {
        memoriaSecundaria[i].pid = -1;
        memoriaSecundaria[i].pagina = -1;
    }

    switch (opcion) {
        case 'f': 
        
            while (scanf("%d\n%d", &pid, &pagina) != EOF) {

                hayLugarEnRAM = false;
                procesoPaginaEnRAM = false;
                procesoPaginaEnDISCO = false;
                indicePaginaEnDisco = 0;

                proceso_t proceso;

                if (procesos[pid-1].pid == -1) { // si el proceso no existe, lo creo e inicializo su tabla de paginas
                    proceso.pid = pid;

                    for (i = 0; i < (MEMORIA_VIRTUAL / TAMANIO_PAGINA); i++) {
                        proceso.tabla_paginas[i] = -1;
                    }

                    procesos[pid-1] = proceso;
                }

                // recupero el proceso
                proceso = procesos[pid-1];

                // pregunto si el proceso y pagina esta en memoria RAM
                for (i = 0; i < (MEMORIA_FISICA / TAMANIO_PAGINA); i++) {
                    if (memoriaFisica[i].pid == pid && memoriaFisica[i].pagina == pagina) {
                        procesoPaginaEnRAM = true;
                        break;
                    }
                }

                // pregunto si el proceso y pagina esta en disco
                for (i = 0; i < (ALMACENAMIENTO_SECUNDARIO / TAMANIO_PAGINA); i++) {
                    if (memoriaSecundaria[i].pid == pid && memoriaSecundaria[i].pagina == pagina) {
                        procesoPaginaEnDISCO = true;
                        break;
                    }
                }

                // pregunto si hay lugar en la RAM
                for (i = 0; i < (MEMORIA_FISICA / TAMANIO_PAGINA); i++) {
                    if (memoriaFisica[i].pid == -1 && memoriaFisica[i].pagina == -1) {
                        hayLugarEnRAM = true;
                        break;
                    }
                }

                if (procesoPaginaEnRAM == true) { // si el proceso y pagina estan en RAM, no hago nada
                    //printf("PROCESO %d y PAGINA %d ESTAN EN RAM\n", pid, pagina);
                    continue;
                } 

                if (procesoPaginaEnRAM == false && procesoPaginaEnDISCO == false && hayLugarEnRAM == true) {
                    // accedo a la pagina
                    proceso.tabla_paginas[pagina-1] = indiceRAM+1;

                    pagina_t marco;
                    marco.pid = proceso.pid;
                    marco.pagina = pagina;
                    memoriaFisica[indiceRAM] = marco;

                    procesos[pid-1] = proceso;
                    indiceRAM = (indiceRAM + 1) % (MEMORIA_FISICA / TAMANIO_PAGINA);
                }
                        
                if (procesoPaginaEnRAM == false && procesoPaginaEnDISCO == false && hayLugarEnRAM == false) { 

                    // bajo a disco el proceso pagina de RAM en 'indiceRAM' a la posicion 'indiceDISCO' del disco
                    pagina_t paginaDisco;
                    paginaDisco.pid = memoriaFisica[indiceRAM].pid;
                    paginaDisco.pagina = memoriaFisica[indiceRAM].pagina;
                    memoriaSecundaria[indiceDISCO] = paginaDisco;
                    indiceDISCO = (indiceDISCO + 1) % (ALMACENAMIENTO_SECUNDARIO / TAMANIO_PAGINA);

                    proceso.tabla_paginas[pagina-1] = indiceRAM+1;

                    pagina_t marco;
                    marco.pid = proceso.pid;
                    marco.pagina = pagina;
                    memoriaFisica[indiceRAM] = marco;

                    procesos[pid-1] = proceso;
                    indiceRAM = (indiceRAM + 1) % (MEMORIA_FISICA / TAMANIO_PAGINA);
                }
                            
                if (procesoPaginaEnRAM == false && procesoPaginaEnDISCO == true && hayLugarEnRAM == false) {
                    
                    for (i = 0; i < (ALMACENAMIENTO_SECUNDARIO / TAMANIO_PAGINA); i++) {
                        if (memoriaSecundaria[i].pid == pid && memoriaSecundaria[i].pagina == pagina) {
                            indicePaginaEnDisco = i;
                            break;
                        }
                    }

                    int auxPidDisco = memoriaSecundaria[indicePaginaEnDisco].pid;
                    int auxPaginaDisco = memoriaSecundaria[indicePaginaEnDisco].pagina;

                    pagina_t paginaDisco;
                    paginaDisco.pid = memoriaFisica[indiceRAM].pid;
                    paginaDisco.pagina = memoriaFisica[indiceRAM].pagina;
                    memoriaSecundaria[indicePaginaEnDisco] = paginaDisco;

                    proceso.tabla_paginas[pagina-1] = indiceRAM+1;
                    procesos[pid-1] = proceso;

                    pagina_t marco;
                    marco.pid = auxPidDisco;
                    marco.pagina = auxPaginaDisco;
                    memoriaFisica[indiceRAM] = marco;
                    
                    indiceRAM = (indiceRAM + 1) % (MEMORIA_FISICA / TAMANIO_PAGINA);
                }
                        
            }

            break;
        
        case 'l':

            while (scanf("%d\n%d", &pid, &pagina) != EOF) {

                hayLugarEnRAM = false;
                procesoPaginaEnRAM = false;
                procesoPaginaEnDISCO = false;
                indicePaginaEnDisco = 0;

                proceso_t proceso;

                if (procesos[pid-1].pid == -1) { // si el proceso no existe, lo creo e inicializo su tabla de paginas
                    proceso.pid = pid;

                    for (i = 0; i < (MEMORIA_VIRTUAL / TAMANIO_PAGINA); i++) {
                        proceso.tabla_paginas[i] = -1;
                    }

                    procesos[pid-1] = proceso;
                }

                // recupero el proceso
                proceso = procesos[pid-1];

                // pregunto si el proceso y pagina esta en memoria RAM
                for (i = 0; i < (MEMORIA_FISICA / TAMANIO_PAGINA); i++) {
                    if (memoriaFisica[i].pid == pid && memoriaFisica[i].pagina == pagina) {
                        procesoPaginaEnRAM = true;
                        indiceProcesoPaginaEnRAM = i;
                        break;
                    }
                }

                // pregunto si el proceso y pagina esta en disco
                for (i = 0; i < (ALMACENAMIENTO_SECUNDARIO / TAMANIO_PAGINA); i++) {
                    if (memoriaSecundaria[i].pid == pid && memoriaSecundaria[i].pagina == pagina) {
                        procesoPaginaEnDISCO = true;
                        break;
                    }
                }

                // pregunto si hay lugar en la RAM
                for (i = 0; i < (MEMORIA_FISICA / TAMANIO_PAGINA); i++) {
                    if (memoriaFisica[i].pid == -1 && memoriaFisica[i].pagina == -1) {
                        hayLugarEnRAM = true;
                        break;
                    }
                }

                if (procesoPaginaEnRAM == true) { // si el proceso y pagina estan en RAM, su tiempo es max+1
                    max = tiempoRAM[0];
                    for (i = 0; i < (MEMORIA_FISICA / TAMANIO_PAGINA); i++) {
                        if (tiempoRAM[i] > max) {
                            max = tiempoRAM[i];
                        }
                    }
                    tiempoRAM[indiceProcesoPaginaEnRAM] = max+1;
                    // AUMENTAR TIEMPO A TODOS LOS PROCESOS PAGINA ???
                    continue;
                } 

                if (procesoPaginaEnRAM == false && procesoPaginaEnDISCO == false && hayLugarEnRAM == true) {

                    for (i = 0; i < (MEMORIA_FISICA / TAMANIO_PAGINA); i++) {
                        if (memoriaFisica[i].pid == -1 && memoriaFisica[i].pagina == -1) {
                            indiceRAM_LRU = i;
                            break;
                        }
                    }

                    // accedo a la pagina
                    proceso.tabla_paginas[pagina-1] = indiceRAM_LRU+1;

                    pagina_t marco;
                    marco.pid = proceso.pid;
                    marco.pagina = pagina;
                    memoriaFisica[indiceRAM_LRU] = marco;
                    tiempoRAM[indiceRAM_LRU] = tiempo;
                    tiempo++;
                    //printf("%d\n", tiempoRAM[indiceRAM_LRU]);

                    procesos[pid-1] = proceso;
                }
                        
                if (procesoPaginaEnRAM == false && procesoPaginaEnDISCO == false && hayLugarEnRAM == false) { 

                    // busco el proceso y pagina menos recientemente usado
                    min = tiempoRAM[0];
                    indiceMin = 0;
                    for (i = 0; i < (MEMORIA_FISICA / TAMANIO_PAGINA); i++) {
                        if (tiempoRAM[i] < min) {
                            min = tiempoRAM[i];
                            indiceMin = i;
                        }
                    }

                    // busco el tiempo del proceso  y pagina mas recientemente utilizado
                    max = tiempoRAM[0];
                    for (i = 0; i < (MEMORIA_FISICA / TAMANIO_PAGINA); i++) {
                        if (tiempoRAM[i] > max) {
                            max = tiempoRAM[i];
                        }
                    }

                    //printf("min: %d\n", min);
                    //printf("indiceMin:%d\n", indiceMin);

                    // bajo a disco el proceso pagina
                    pagina_t paginaDisco;
                    paginaDisco.pid = memoriaFisica[indiceMin].pid;
                    paginaDisco.pagina = memoriaFisica[indiceMin].pagina;
                    memoriaSecundaria[indiceDISCO] = paginaDisco;
                    indiceDISCO = (indiceDISCO + 1) % (ALMACENAMIENTO_SECUNDARIO / TAMANIO_PAGINA);

                    proceso.tabla_paginas[pagina-1] = indiceMin+1;

                    tiempoRAM[indiceMin] = max+1;

                    pagina_t marco;
                    marco.pid = proceso.pid;
                    marco.pagina = pagina;
                    memoriaFisica[indiceMin] = marco;

                    // incremento el tiempo de todos los procesos en 1
                    for (i = 0; i < (MEMORIA_FISICA / TAMANIO_PAGINA); i++) {
                        tiempoRAM[i] = tiempoRAM[i] + 1;
                        //printf("%d\n", tiempoRAM[i]);
                    }

                    procesos[pid-1] = proceso;
                    
                }
                            
                if (procesoPaginaEnRAM == false && procesoPaginaEnDISCO == true && hayLugarEnRAM == false) {
                    for (i = 0; i < (ALMACENAMIENTO_SECUNDARIO / TAMANIO_PAGINA); i++) {
                        if (memoriaSecundaria[i].pid == pid && memoriaSecundaria[i].pagina == pagina) {
                            indicePaginaEnDisco = i;
                            //printf("%d.%d indice: %d\n", memoriaSecundaria[i].pid, memoriaSecundaria[i].pagina, indicePaginaEnDisco);
                            break;
                        }
                    }
                    

                    int auxPidDisco = memoriaSecundaria[indicePaginaEnDisco].pid;
                    int auxPaginaDisco = memoriaSecundaria[indicePaginaEnDisco].pagina;
                    //printf("aux %d.%d\n", auxPidDisco, auxPaginaDisco);

                    min = tiempoRAM[0];
                    indiceMin = 0;
                    for (i = 0; i < (MEMORIA_FISICA / TAMANIO_PAGINA); i++) {
                        //printf("%d\n", tiempoRAM[i]);
                        if (tiempoRAM[i] < min) {
                            min = tiempoRAM[i];
                            indiceMin = i;
                        }
                    }

                    //printf("indiceMin:%d\n", indiceMin);

                    pagina_t paginaDisco;
                    paginaDisco.pid = memoriaFisica[indiceMin].pid;
                    paginaDisco.pagina = memoriaFisica[indiceMin].pagina;
                    //printf("pagina disco %d.%d\n", memoriaFisica[indiceMin].pid, memoriaFisica[indiceMin].pagina);
                    memoriaSecundaria[indicePaginaEnDisco] = paginaDisco;

                    proceso.tabla_paginas[pagina-1] = indiceMin+1;
                    procesos[pid-1] = proceso;

                    pagina_t marco;
                    marco.pid = auxPidDisco;
                    marco.pagina = auxPaginaDisco;
                    memoriaFisica[indiceMin] = marco;

                    // busco el tiempo del proceso  y pagina mas recientemente utilizado
                    max = tiempoRAM[0];
                    for (i = 0; i < (MEMORIA_FISICA / TAMANIO_PAGINA); i++) {
                        if (tiempoRAM[i] > max) {
                            max = tiempoRAM[i];
                        }
                    }

                    tiempoRAM[indiceMin] = max+1;

                    // incremento el tiempo de todos los procesos en 1
                    for (i = 0; i < (MEMORIA_FISICA / TAMANIO_PAGINA); i++) {
                        //printf("post swap: %d\n", tiempoRAM[i]);
                        tiempoRAM[i] = tiempoRAM[i] + 1;
                    }
                    
                }
                        
            }

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
                if (procesos[j].tabla_paginas[k] == -1) {
                    printf("- ");
                } else {
                    printf("%d ", procesos[j].tabla_paginas[k]);
                }
            }
            printf("\n");
        }
    }

    // Imprimo la memoria fisica
    j = 0;
    printf("Memoria fisica: ");
    for (j = 0; j < (MEMORIA_FISICA / TAMANIO_PAGINA); j++) {

        if (memoriaFisica[j].pid == -1 && memoriaFisica[j].pagina == -1) {
            printf("- ");
        } else {
            printf("%d.%d ", memoriaFisica[j].pid, memoriaFisica[j].pagina);
        }
    }
    printf("\n");

    // Imprimo la memoria secundaria
    j = 0;
    printf("Memoria secundaria: ");
    for (j = 0; j < (ALMACENAMIENTO_SECUNDARIO / TAMANIO_PAGINA); j++) {

        if (memoriaSecundaria[j].pid == -1 && memoriaSecundaria[j].pagina == -1) {
            printf("- ");
        } else {
            printf("%d.%d ", memoriaSecundaria[j].pid, memoriaSecundaria[j].pagina);
        }
    }
    printf("\n");

    exit(EXIT_SUCCESS);
}
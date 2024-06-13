#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <path_archivo>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct stat fileStat;

    if (stat((argv[1]), &fileStat) < 0) {
        fprintf(stderr, "Error al recuperar la metadata del archivo %s", argv[1]);
        exit(EXIT_FAILURE);
    }

    printf("ID del usuario propietario: %d\n", fileStat.st_uid);
    printf("El tipo de archivo: %d\n", fileStat.st_mode);
    printf("Número de i-nodo: %d\n", (int) fileStat.st_ino);
    printf("Tamaño en bytes: %d\n", (int) fileStat.st_size);
    printf("Número de bloques que ocupa en disco: %d\n", (int) fileStat.st_blocks);
    printf("Tiempo de última modificación: %s", ctime( (time_t*) (&fileStat.st_mtim)));
    printf("Tiempo de último acceso: %s", ctime( (time_t*) (&fileStat.st_atim)));
    
    exit(EXIT_SUCCESS);
}
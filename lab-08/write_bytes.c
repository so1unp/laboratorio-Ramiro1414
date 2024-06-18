#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

int main(int argc, char *argv[])
{
    size_t bufSize, numBytes, thisWrite, totWritten;
    char *buf;
    int sync, fd, openFlags;
    // clock_t start = clock();
    // float elapsed_time;

    if (argc != 5) {
        fprintf(stderr, "Uso: %s num-bytes buf-size sync archivo\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // start = clock();

    numBytes = atoi(argv[1]);
    bufSize = atoi(argv[2]);
    sync = atoi(argv[3]);

    buf = malloc(bufSize);
    if (buf == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    openFlags = O_CREAT | O_WRONLY;

    fd = open(argv[4], openFlags, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Esta condicion esta presente para evitar el warning que dice que sync no esta siendo usada.
    // Se puede eliminar cuando se implemente fsync() y fdatasync()
    // sync = sync == 1 ? 0 : 1;

    for (totWritten = 0; totWritten < numBytes; totWritten += thisWrite) {
        thisWrite = numBytes - totWritten > bufSize ? bufSize : numBytes - totWritten;

        if (write(fd, buf, thisWrite) != thisWrite) {
            perror("write");
            exit(EXIT_FAILURE);
        }
        if (sync == 1) {
            fsync(fd);
        }
        if (sync == 2) {
            fdatasync(fd);
        }
    }

    if (close(fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    // clock_t end = clock();

    // elapsed_time = ( ((float) (end - start)) / CLOCKS_PER_SEC);

    // printf("El programa tardó %f segundos en ejecutarse.\n", elapsed_time);

    exit(EXIT_SUCCESS);
}

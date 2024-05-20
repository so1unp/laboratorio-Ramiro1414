#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <string.h>
#include <unistd.h>

#define USERNAME_MAXSIZE    15  // Máximo tamaño en caracteres del nombre del remitente.
#define TXT_SIZE            100 // Máximo tamaño del texto del mensaje.
#define QUEUE_PERMISSIONS 0666
#define MAX_MSG 10 // Maximo numero de mensajes en la cola.

/**
 * Estructura del mensaje:
 * - sender: nombre del usuario que envió el mensaje.
 * - text: texto del mensaje.
 */
struct msg {
    char sender[USERNAME_MAXSIZE];
    char text[TXT_SIZE];
};

typedef struct msg msg_t;

/**
 * Imprime información acerca del uso del programa.
 */
void usage(char *argv[])
{
    fprintf(stderr, "Uso: %s comando parametro\n", argv[0]);
    fprintf(stderr, "Comandos:\n");
    fprintf(stderr, "\t-s queue mensaje: escribe el mensaje en queue.\n");
    fprintf(stderr, "\t-r queue: imprime el primer mensaje en queue.\n");
    fprintf(stderr, "\t-a queue: imprime todos los mensaje en queue.\n");
    fprintf(stderr, "\t-l queue: vigila por mensajes en queue.\n");
    fprintf(stderr, "\t-c queue: crea una cola de mensaje queue.\n");
    fprintf(stderr, "\t-d queue: elimina la cola de mensajes queue.\n");
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

    switch(option) {
        case 's':
            printf("Enviar %s a la cola %s\n", argv[2], argv[3]);
            
            // recupero la cola de mensajes y sus atributos
            mqd_t queue = mq_open(argv[3], O_RDWR);
            struct mq_attr attr;
            mq_getattr(queue, &attr);

            // creo el mensaje
            msg_t message;
            getlogin_r(message.sender, sizeof(message.sender));
            strncpy(message.text, argv[2], sizeof(message.text) - 1);
            mq_send(queue, (char*) &message, attr.mq_msgsize, 0);
            
            mq_close(queue);

            break;
        case 'r':
            printf("Recibe el primer mensaje en %s\n", argv[2]);

            msg_t receivedMessage;

            queue = mq_open(argv[2], O_RDONLY);
            mq_getattr(queue, &attr);

            // verifico si hay mensajes
            if (attr.mq_curmsgs == 0) {
                printf("No hay mensajes.\n");
                exit(EXIT_SUCCESS);
            }

            // Creo buffer para recibir el mensaje
            char *buffer = (char *)malloc(attr.mq_msgsize);
            if (buffer == NULL) {
                fprintf(stderr, "Error al hacer malloc.\n");
                mq_close(queue);
                exit(EXIT_FAILURE);
            }

            // Recibo el mensaje
            ssize_t bytesReceived;
            bytesReceived = mq_receive(queue, buffer, attr.mq_msgsize, NULL);
            if (bytesReceived == -1) {
                fprintf(stderr, "Error al hacer mq_receive.\n");
                free(buffer);
                mq_close(queue);
                exit(EXIT_FAILURE);
            }

            memcpy(&receivedMessage, buffer, sizeof(receivedMessage));

            printf("%s: %s\n", receivedMessage.sender, receivedMessage.text);

            free(buffer);
            mq_close(queue);

            break;
        case 'a':
            printf("Imprimer todos los mensajes en %s\n", argv[2]);

            queue = mq_open(argv[2], O_RDONLY);
            mq_getattr(queue, &attr);

            //printf("hay %d mensajes\n", (int) attr.mq_curmsgs);

            if (attr.mq_curmsgs == 0) {
                printf("No hay mensajes.\n");
                exit(EXIT_SUCCESS);
            }

            int numberOfMessages = attr.mq_curmsgs;
            buffer = (char *)malloc(attr.mq_msgsize);

            while (numberOfMessages != 0) {

                bytesReceived = mq_receive(queue, buffer, attr.mq_msgsize, NULL);
                if (bytesReceived == -1) {
                    fprintf(stderr, "Error al hacer mq_receive.\n");
                    free(buffer);
                    mq_close(queue);
                    exit(EXIT_FAILURE);
                }

                memcpy(&receivedMessage, buffer, sizeof(receivedMessage));

                printf("%s: %s\n", receivedMessage.sender, receivedMessage.text);

                numberOfMessages--;
            }

            free(buffer);
            mq_close(queue); 

            break;
        case 'l':
            printf("Escucha indefinidamente por mensajes\n");

            queue = mq_open(argv[2], O_RDONLY);
            mq_getattr(queue, &attr);
            buffer = (char *)malloc(attr.mq_msgsize);

            while (1) {

                bytesReceived = mq_receive(queue, buffer, attr.mq_msgsize, NULL);
                if (bytesReceived == -1) {
                    fprintf(stderr, "Error al hacer mq_receive.\n");
                    free(buffer);
                    mq_close(queue);
                    exit(EXIT_FAILURE);
                }

                memcpy(&receivedMessage, buffer, sizeof(receivedMessage));

                printf("%s: %s\n", receivedMessage.sender, receivedMessage.text);

            }

            break;
        case 'c':
            printf("Crea la cola de mensajes %s\n", argv[2]);
            
            attr.mq_maxmsg = MAX_MSG; // maximo numero de mensajes en la cola
            attr.mq_msgsize = USERNAME_MAXSIZE + TXT_SIZE;

            queue = mq_open(argv[2], O_CREAT, QUEUE_PERMISSIONS, &attr);

            /*
            if (mq_open(argv[2], O_CREAT, QUEUE_PERMISSIONS, &attr) < 0) {
                fprintf(stderr, "Error al crear la cola de mensajes.\n");
                exit(EXIT_FAILURE);
            }
            */

            mq_close(queue);
            
            break;
        case 'd':
            printf("Borra la cola de mensajes %s\n", argv[2]);

            if (mq_unlink(argv[2]) < 0) {
                fprintf(stderr, "Error al eliminar la cola de mensajes.\n");
                exit(EXIT_FAILURE);
            }
            
            break;
        case 'h':
            usage(argv);
            break;
        default:
            fprintf(stderr, "Comando desconocido: %s\n", argv[1]);
            exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}

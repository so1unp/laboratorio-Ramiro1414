# Laboratorio 5 - Sincronizacion y comunicación

## Ejercicio 1: buffer limitado

El programa `buf.c` implementa un ejemplo de productor-consumidor haciendo uso de un _buffer limitado_. El programa no utiliza mecanismos de sincronización para el acceso al buffer. Esto ocasiona condiciones de carrera. Modificar el programa para sincronizar el acceso al buffer, empleando semáforos y _mutexs_.

Utilizar las siguientes funciones:
* Crear un mutex: [`pthread_mutex_init()`](http://man7.org/linux/man-pages/man3/pthread_mutex_init.3p.html)
* Inicializar un semáforo: [`sem_init()`](https://man7.org/linux/man-pages/man3/sem_init.3.html)
* Tomar un semáforo: [`sem_wait()`](https://man7.org/linux/man-pages/man3/sem_wait.3.html)
* Liberar un semáforo: [`sem_post()`](https://man7.org/linux/man-pages/man3/sem_post.3.html)
* Tomar un mutex: [`pthread_mutex_lock()`](https://www.man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html)
* Liberar un mutex: [`pthread_mutex_unlock()`](https://www.man7.org/linux/man-pages/man3/pthread_mutex_unlock.3p.html)
* Eliminar un semáforo: [`sem_destroy()`](https://man7.org/linux/man-pages/man3/sem_destroy.3.html)
* Eliminar un mutex: [`pthread_mutex_destroy()`](https://www.man7.org/linux/man-pages/man3/pthread_mutex_destroy.3p.html)

## Ejercicio 2: paso de mensajes

El objetivo del ejercicio es crear un programa que permita intercambiar mensajes entre los usuarios del sistema. Para esto vamos a utilizar la librería de mensajes POSIX.

El programa `msgbox.c` contiene un esqueleto del programa, que procesa los siguientes parámetros pero no hace más que imprimir un mensaje:

- `-s queue msg`: envía el mensaje `msg` a la cola `queue`.
- `-r queue`: imprime el primer mensaje en `queue`.
- `-a queue`: imprime todos los mensaje en `queue`.
- `-l queue`: el programa espera por mensajes en `queue`.
- `-c queue`: crea la cola de mensaje `queue`.
- `-d queue`: elimina la cola de mensajes `queue`.
- `-i queue`: imprime información de la cola de mensajes `queue` (máximo número de mensajes, tamaño de los mensajes, número actual de mensajes en la cola).

La estructura de datos que representa un mensaje es `struct msg`, que contiene el nombre del usuario que envía el mensaje (`sender`) y el mensaje (`text`). Notar que lo que se debe enviar como mensaje es la estructura completa, no solamente el campo `text`.

```c
#define USERNAME_SIZE   15
#define TXT_SIZE        100

/**
 * Estructura del mensaje:
 * - sender: nombre del usuario que envió el mensaje.
 * - text: texto del mensaje.
 */
struct msg {
    char sender[USERNAME_SIZE];
    char text[TXT_SIZE];
};

typedef struct msg msg_t;
```

Para crear un buzón de mensajes, enviar y recibir mensajes, usar las siguientes funciones:

* [`mq_open()`](http://man7.org/linux/man-pages/man3/mq_open.3.html): crea una nueva cola de mensajes o abre una ya existente.
* [`mq_send()`](http://man7.org/linux/man-pages/man3/mq_send.3.html): envía un mensaje a la cola de mensajes.
* [`mq_receive()`](http://man7.org/linux/man-pages/man3/mq_receive.3.html): recibe un mensaje.
* [`mq_close()`](http://man7.org/linux/man-pages/man3/mq_close.3.html): cierra el descriptor de una cola de mensajes.
* [`mq_unlink()`](http://man7.org/linux/man-pages/man3/mq_unlink.3.html): elimina una cola de mensajes.
* [`getlogin_r`](https://www.man7.org/linux/man-pages/man3/getlogin.3.html): obtiene el nombre del usuario.

Una vez completado el programa, deben poder crear colas de mensajes y envíar y recibir mensajes por medio de las mismas utilizando el comando `bin/msgbox`:

![Ejemplo de uso de Msg](stuff/msgbox.gif)

## Ejercicio 3: canvas usando memoria compartida

Vamos a armar un _canvas_, un espacio de memoria compartida cuyo contenido puede ser modificado por distintos usuarios. Este espacio es un arreglo de 25x25 caracteres, sobre el cual se pueden escribir palabras en alguna posición. La estructura de datos que representa el _canvas_ es `canvas_t`:

```c
#define HEIGHT  25  // Altura en caracteres de la pizarra
#define WIDTH   25  // Ancho en caracteres de la pizarra

struct canvas {
    char canvas[HEIGHT*WIDTH];
};

typedef struct canvas canvas_t;
```

Para crear y acceder al _canvas_ necesitamos utilizar memoria compartida. Para lograrlo, usaremos el API de POSIX para crear y utilizar segmentos de memoria compartida. Mediante estos segmentos, diferentes procesos pueden intercambiar datos de una manera más rapida que mediante el uso de mensajes. El manual [`shm_overview`](http://man7.org/linux/man-pages/man7/shm_overview.7.html) tiene una introducción al API de memoria compartida de POSIX.

Completar el programa `canvas.c`, de manera que se puedan crear, eliminar, imprimir y modificar estos _canvas_. Las principales funciones que vamos a usar son:

* [`shm_open()`](http://man7.org/linux/man-pages/man3/shm_open.3.html): crea un nuevo objeto de memoria compartida, o abre uno ya existente.
* [`ftruncate()`](http://man7.org/linux/man-pages/man2/ftruncate.2.html): cambia ("trunca") el tamaño del segmento de memoria compartida.
* [`mmap()`](http://man7.org/linux/man-pages/man2/mmap.2.html): mapea el segmento de memoria compartida indicado dentro del espacio de direcciones del proceso.
* [`close()`](http://man7.org/linux/man-pages/man2/close.2.html): cierra el descriptor de un segmento de memoria compartida.
* [`shm_unlink()`](http://man7.org/linux/man-pages/man3/shm_unlink.3.html): elimina el segmento de memoria compartida indicado.

## Ejercicio 4: memoria compartida

Completar el programa `wordstack.c` que permite generar y/o acceder una zona memoria compartida que almacena una pila de palabras. La estructura de datos que representa la pila es `stack`:

```c
#define ITEMS       15
#define MAX_WORD    50

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
```

El programa `wordstack.c` ya cuenta con el código necesario para reconocer las siguientes opciones:

* `-c`: Crear una nueva pila de palabras (zona de memoria compartida).
* `-w`: Escribe una palabra en el siguiente lugar libre.
* `-r`: Recupera y elimina la palabra en el tope de la pila.
* `-p`: Imprime la pila de palabras.
* `-d`: Elimina la pila de palabras (zona de memoria compartida).

Completar el programa de manera que se puedan crear, eliminar, imprimir y modificar una pila de palabras. Para esto usaremos el API de POSIX para crear y utilizar segmentos de memoria compartida (el manual [`shm_overview`](http://man7.org/linux/man-pages/man7/shm_overview.7.html) tiene una introducción a esta API). Las funciones que se utilizarán son:

* [`shm_open()`](http://man7.org/linux/man-pages/man3/shm_open.3.html): crea un nuevo objeto de memoria compartida, o abre uno ya existente.
* [`ftruncate()`](http://man7.org/linux/man-pages/man2/ftruncate.2.html): cambia ("trunca") el tamaño del segmento de memoria compartida.
* [`mmap()`](http://man7.org/linux/man-pages/man2/mmap.2.html): mapea el segmento de memoria compartida indicado dentro del espacio de direcciones del proceso.
* [`close()`](http://man7.org/linux/man-pages/man2/close.2.html): cierra el descriptor de un segmento de memoria compartida.
* [`shm_unlink()`](http://man7.org/linux/man-pages/man3/shm_unlink.3.html): elimina el segmento de memoria compartida indicado.

Una vez este funcionando la pila, agregar el control de sincronización y exclusión mutua necesario para evitar condiciones de carrera cuando dos o más procesos accedan al mismo tiempo a la zona de memoria compartida:

* Solo un proceso a la vez puede modificar la pila (usando el `mutex`).
* Si la pila esta llena `-c` se bloquea hasta que pueda escribir la palabra.
* Si la pila esta vacía `-r` se bloquea hasta que puede leer y remover una palabra.
* Para que otros usuarios puedan acceder a la pila, la zona de memoria compartida debe permitir que otros usuarios la puedan modificar y los semáforos deben permitir ser compartidos entre procesos.

---

¡Fin del Laboratorio 5!

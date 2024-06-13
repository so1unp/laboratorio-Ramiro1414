# Laboratorio 8 - Sistema de Archivos

## Ejercicio 1: Montaje de un sistema de archivos

- Crear un directorio dentro de la carpeta personal de nombre `tmpfs` y luego un archivo vacío dentro del mismo:
    ```console
    $ mkdir ~/tmpfs
    $ touch ~/tmpfs/archivo
    ```

- Listar el contenido del directorio `~/tmpfs` usando la opción `-i` de `ls`, que muestra el número de i-nodo. Deberiamos ver algo similar a lo siguiente:
    ```console
    $ ls -lhia ~/tmpfs/ 
    total 8,0K
    13369966 drwxrwxr-x  2 fep fep 4,0K jun 12 15:57 .
    12582915 drwxr-x--- 30 fep fep 4,0K jun 12 16:04 ..
    13371770 -rw-rw-r--  1 fep fep    0 jun 12 15:57 archivo
    $
    ```

- Montar en el directorio `~tmpfs` un sistema de archivos del tipo `tmpfs` que funciona completamente en memoria. Para ejecutar el comando [`mount`](https://www.man7.org/linux/man-pages/man8/mount.8.html) se requieren permisos de superusuario, por lo cual usamos el comando `sudo`. La opción `-t` indica el tipo de sistemas de archivo que deseamos montar. Deberíamos ver que el contenido anterior del directorio ya no esta disponible:
    ```console
    $ sudo mount -t tmpfs -o size=16MiB temporal ~/tmpfs
    $ ls -lhia ~/tmpfs
    total 4,0K
           1 drwxrwxrwt  2 root root   40 jun 12 16:05 .
    12582915 drwxr-x--- 30 fep  fep  4,0K jun 12 16:05 ..
    $
    ```

- Ejecutar el comando `cat /proc/mounts`, para listar los puntos de montaje actuales en el sistema. En la última línea deberiamos ver algo similar a lo siguiente:
    ```console
    temporal /home/fep/tmpfs tmpfs rw,relatime,inode64 0 0
    ```

- Crear un archivo dentro del directorio `~/tmpfs` y volver a listar el contenido del mismo:
    ```console
    $ touch ~/tmpfs/archivo
    $ ls -lhia ~/tmpfs
    total 4,0K
           1 drwxrwxrwt  2 root root   60 jun 12 16:07 .
    12582915 drwxr-x--- 30 fep  fep  4,0K jun 12 16:07 ..
           2 -rw-rw-r--  1 fep  fep     0 jun 12 16:07 archivo
    $
    ```
    * ¿Por qué el número de i-nodo en el punto de montaje inicia en 1?

- Crear un archivo de 16 MiB en el directorio `~/tmpfs`. Por ejemplo, se puede crear utilizando el comando `head`. Si ejecutamos el comando `ls` sin la opción `-h` nos presenta el tamaño de los archivos y directorios expresados en bytes.
    ```console
    $ head -c 16MiB /dev/urandom > ~/tmpfs/big
    $ ls -lia ~/tmpfs
    total 16388
           1 drwxrwxrwt  2 root root      120 jun 12 16:07 .
    12582915 drwxr-x--- 30 fep  fep      4096 jun 12 16:07 ..
           2 -rw-rw-r--  1 fep  fep         0 jun 12 16:07 archivo
           3 -rw-rw-r--  1 fep  fep  16777216 jun 12 16:23 big
    $
    ```

- Probar ejecutar los siguientes comandos, en el orden indicado:
    ```console
    $ touch ~/tmpfs/a.txt
    $ echo "hola" > ~/tmpfs/b.txt
    $ touch ~/tmpfs/b.txt
    ```
    - ¿Por qué los comandos `touch` funcionan correctamente y el comando `echo` falla?

- Para desmontar el sistema de archivos, ejecutamos el comando `umount` indicando el nombre del punto de montaje. Podemos comprobar que el contenido original del directorio `~/tmpfs` vuelve a estar disponible:
    ```console
    $ sudo umount temporal
    $ ls -lhia ~/tmpfs/ 
    total 8,0K
    13369966 drwxrwxr-x  2 fep fep 4,0K jun 12 15:57 .
    12582915 drwxr-x--- 30 fep fep 4,0K jun 12 16:04 ..
    13371770 -rw-rw-r--  1 fep fep    0 jun 12 15:57 archivo
    $ 
    ```

## Ejercicio 2: Atributos de archivos

Completar el programa `fstat.c` para que imprima los siguientes datos del archivo indicado:
- ID del usuario propietario.
- El tipo de archivo (ver la sección _The File Type and Mode_ en `man 7 inode`).
- Número de i-nodo.
- Tamaño en bytes.
- Número de bloques que ocupa en disco.
- Tiempo de última modificación y acceso (usar la función [`ctime()`](https://www.man7.org/linux/man-pages/man3/ctime.3.html) para dar formato).

## Ejercicio 3: Perfomance de E/S

El programa `test_write.c` crea un archivo y escribe en el mismo la cantidad de bytes indicada. Por ejemplo:
```console
$ bin/test_write 16777216 256 0 archivo.txt
```
Crea el archivo `archivo.txt` y escribe en el mismo 16777216 bytes (16 MiB) usando un buffer de 256 bytes al invocar la llamada al sistema `write()`.

1. Evaluar los tiempos de ejecución del comando al generar un archivo de 25 MiB usando los siguientes tamaños de buffer: 1024, 8192 y 32768. Realizar varias ejecuciones de cada comando para obtener un promedio.

2. Implementar la opción `sync` (tercer parámetro). Si es 0, no se realiza ninguna acción luego de cada `write()`. Si es `1` se debe invocar [`fsync()`](https://man7.org/linux/man-pages/man2/fdatasync.2.html) luego de cada `write()`. Si es `2`, se debe invocar [`fdatasync()`](https://man7.org/linux/man-pages/man2/fdatasync.2.html).

3. Evaluar los tiempos de ejecución usando las opciones `1` y `2`.

4. Presentar una tabla comparativa de los tiempos de ejecución y justificar los resultados.

---

¡Fín del Laboratorio!

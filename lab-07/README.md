# Laboratorio 7 - Administración de Memoria

## Ejercicio 1: simulador de paginación y memoria virtual

Implementar en el programa `sim.c` un simulador de paginación y memoria virtual, que simule una computadora con 64 KiB de memoria virtual, memoria física de 32 KiB, un tamaño de las páginas de 4 KiB y 64 KiB de espacio en almacenamiento secundario para páginas que se desalojen. El parámetro que debe aceptar el programa es el tipo de algoritmo de reemplazo de páginas: `-f` (FIFO) o `-l` (LRU).

El simulador debe leer desde la entrada estándar un identificador del proceso y el número de página que modifica, uno por línea. A medida que va leyendo, realiza la asignación en memoria, si es posible. Debe finalizar al leer un EOF.

Por ejemplo, la siguiente entrada indica que el proceso 1 accede a las páginas 3 y 9, y el proceso 2 a la página 1.
```
1
3
1
9
1
3
2
1
```

Una vez finalizada la ejecución el simulador debe imprimir:

- La tabla de páginas de cada proceso. El formato debe ser el siguiente (usando el ejemplo anterior):
    - Proceso 1: `- - 1 - - - - - 2 - - - - - - - -`
    - Proceso 2: `3 - - - - - - - - - - - - - - - -`
- La memoria física. Mismo formato que las tablas de páginas, pero cada entrada debe ser PID.PAGINA. Para el ejemplo anterior:
    - `1.3 1.9 2.1 - - - - -`
    - Indica que en el primer marco se almaceno la tercer página del proceso 1.
- El almacenamiento secundario. Mismo formato que la memoría física. Para el ejemplo anterior:
    - `- - - - - - - - - - - - - - - - -`

## Ejercicio 2: mapa de memoria de un proceso

En este ejercicio analizaremos la memoria de un proceso en ejecución. Usaremos el programa `maps.c` que utiliza `fork()` para crear un proceso hijo. Ambos procesos esperan recibir dos señales `SIGUSR1`. De esta manera el proceso queda bloqueado a la espera de la señal y nos permite estudiar como estan mapeados en memoria.

### Actividades

Adjuntar las respuestas a las actividades en un documento PDF.

1. Compilar y ejecutar el programa `maps.c` en una terminal, donde queda esperando por el arribo de una señal.

2. Desde otra terminal, ejecutar `htop`. Luego, presionar `F4` (filtrar) y escribir `bin/maps` y dar Enter. De esta manera, sólo se verán los dos procesos que estan ejecutando `maps.c`. Presionar también `F5`, que presenta más claramente la relación entre proceso padre e hijo.
    - La columna `RES` indica cuantas páginas ocupa el proceso en la memoria física. Y la columna `SHR` cuántas están compartidas. ¿Por qué ambos valores son cero en el proceso hijo?

3. Salir de `htop` y ejecutar el comando `pmap -p $(pgrep maps)`. Como resultado debe mostrar información de ambos procesos (padre e hijo).
    - Notar que las direcciones de memoria son idénticas en ambos procesos. ¿Por qué?

4. A continuación, ejecutar `pmap -p $(pgrep maps) | grep maps`, que muestra los mapeos de memoria correspondientes al ejecutable de nuestro programa.
    - ¿Por qué una de las páginas esta marcada como de ejecución?
    - ¿Por qué otra de las páginas permite la escritura?

5. A continuación, ejecutar `pmap -p -XX $(pgrep maps) > maps1.txt`. Como este comando genera mucha más información redirigimos la salida al archivo `maps.txt` para leer más comodamente los resultados en un editor de texto.

6. Abrir el archivo `maps1.txt` y buscar el mapeo del _heap_ y de la página marcada como de lectura/escritura en el punto 3.
    - Según indica la columna `Shared_Dirty` ambos mapeos estan *compartidos*. ¿Por qué?

7. Eliminar ambos procesos ejecutando `kill -15 $(pgrep maps)`.

8. Modificar `maps.c` de manera que el proceso hijo modifique la variable global `a` y también el _heap_.

9. Ejecutar nuevamente `maps` en una terminal.

10. Desde la otra terminal, repetir los puntos 4 y 5 (tendría que mostrar identicos resultados).

11. Ejecutar `kill -s SIGUSR1 $(pgrep maps)` de manera que ambos procesos continuen su ejecución.

12. Ejecutar `pmap -p -XX $(pgrep maps) > maps2.txt`.
    - La columna `Shared_Dirty` del _heap_ y de la pagina marcada como de escritura son cero en ambos procesos. ¿Por qué?

13. Finalmente, ejecutar `kill -s SIGUSR1 $(pgrep maps)` de manera que ambos procesos finalicen su ejecución.

## Ejercicio 3: contador de páginas
Agregar a _xv6_ una llamada al sistema `pgcnt()`, que retorne la cantidad de páginas que ocupa actualmente el proceso en memoria. El código de la llamada al sistema es el siguiente (lo pueden encontrar también en el archivo `sys_pgcnt.c`):
```
int
sys_pgcnt(void)
{
    pde_t *pde;  // Puntero a una entrada en la tabla de directorio.
    pte_t *pte;  // Puntero a una entrada en una tabla de páginas.

    int i = 0;
    int j = 0;
    int cnt = 0;

    // PTE_P: Si la página está en memoria.
    // PTE_U: Si la página corresponde a un proceso de usuario.
    unsigned int flag = PTE_P | PTE_U;

    for(i=0; i < NPDENTRIES; i++)
        // i-ésima entrada en la tabla de directorio.
        pde = &((myproc()->pgdir)[i]);

        // Si no es NULL y hay páginas en memoria
        if(*pde & PTE_P) {
            pte = (pte_t*) P2V(PTE_ADDR(*pde));

            // Recorre cada entrada de la tabla secundaria
            for(j=0; j < NPTENTRIES; j++) {
                // Si esta en memoria la página, la cuenta
                if((pte[j] & flag) == flag) {
                   cnt = cnt + 1;
                }
            }
        }
    }

    return cnt;
}
```

El archivo `pgcnt.c` es un programa de usuario que invoca la llamada al sistema `pgcnt()`. Deben agregarlo a la lista `UPROGS` en el `Makefile` de _xv6_.

Este programa invoca la llamada al sistema `pgcnt()` tres veces. La primera al inicio del programa y luego de invocar a `sbrk` y `malloc`:
```
$ pgcnt
2
3
11
$
```

### Responder:
* Explicar el resultado del programa, justificando el número de páginas que cada invocación a `pgcnt()` retorna. ¿Por qué la diferencia entre las invocaciones de `sbrk` y `malloc`?

---

¡Fín del Laboratorio!

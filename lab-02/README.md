# Laboratorio 2 - Llamadas al Sistema

En este laboratorio vamos a trabajar con las llamadas al sistema relacionadas con archivos y creación de procesos. Además, se verá la implementación de las llamadas al sistema operativo _xv6_.

Las respuestas a las preguntas planteadas en los ejercicios deben ser entregadas en un archivo PDF en el campus virtual.

## Ejercicio 1

El programa `hola.c` imprime el mensaje `¡Hola Mundo!` en la _salida estándar_ utilizando la función de biblioteca [`puts()`](http://man7.org/linux/man-pages/man3/puts.3.html).

Compilar el programa y ejecutarlo mediante el comando `strace`, como se indica a continuación, para obtener las llamadas al sistema que utiliza durante su ejecución:

```console
$ make hola
$ strace bin/hola > /dev/null
```

**Nota**: `> /dev/null` redirije la _salida estándar_ de `bin/hola` al archivo especial del sistema `/dev/null`, que descarta todo lo que se escriba en el mismo. De esta manera evitamos que la salida del comando `hola` se mezcle con la de `strace`.

Responder:

1. ¿Cuantas llamadas al sistema invoca el programa?
2. ¿Cúales son las llamadas al sistema que invocan las funciones de biblioteca `puts()` y `exit()`?
3. Describir los parámetros que utiliza la llamada al sistema invocada por `puts()`.

## Ejercicio 2: Interprete de comandos

El programa `sh.c` es un interprete de comandos (_shell_) que no tiene implementada la funcionalidad de ejecución de programas o de redirección de entrada/salida. Al ejecutarlo, imprime un símbolo de sistema (`$`) y espera ordenes. Para terminar su ejecución teclear `^C`.

### 2.1: Ejecución de comandos

Implementar la ejecución de comandos. El programa genera una estructura `execcmd` que contiene el comando a ejecutar y sus parámetros (si los hubiera). Para implementar la ejecución de comandos, deben completar el caso `EXEC` en la función `runcmd()`, utilizando la llamada a sistema [`exec()`](http://man7.org/linux/man-pages/man3/exec.3.html). Se debe imprimir un mensaje de error si `exec()` falla, utilizando la función [`perror()`](http://man7.org/linux/man-pages/man3/perror.3.html).

### 2.2: Redirección de E/S

Implementar redirección de E/S mediante los operadores `<` y `>`, de manera que el _shell_ permita ejecutar comandos como:

```console
$ echo "sistemas operativos" > so.txt
$ cat < so.txt
sistemas operativos
$
```

El parser implementado en el _shell_ ya reconoce estos operadores y genera una estructura `redircmd` con los datos necesarios para implementar la redirección. Deben completar el código necesario en el caso `REDIR` de la función `runcmd()`. Consultar las llamadas al sistema [`open()`](http://man7.org/linux/man-pages/man2/open.2.html) y [`close()`](http://man7.org/linux/man-pages/man2/close.2.html). Imprimir un mensaje de error si alguna de las llamadas al sistema, utilizando [`perror()`](http://man7.org/linux/man-pages/man3/perror.3.html). Verificar los permisos con los que se crea el archivo.

## Ejercicio 3 - Shell de xv6

En el directorio `xv6` van a encontrar el código del sistema operativo _xv6_, una versión muy reducida de Unix. Parados en dicho directorio, pueden ejecutarlo utilizando la maquina virtual *QEMU* ejecutando el comando `make qemu-nox`. Verían algo similar a lo siguiente en la consola:

```console
Booting from Hard Disk..xv6...
cpu0: starting 0
sb: size 1000 nblocks 941 ninodes 200 nlog 30 logstart 2 inodestart 32 bmap start 58
init: starting sh
$
```

Si ejecutamos un comando como `ls` veremos un error, ya que no está implementada la ejecución de comandos:
```console
$ ls
exec not implemented
$
```

Para terminar la ejecución de QEMU, presionar `C^A x` (esto es, `Ctrl+A` y luego la tecla `x`).

Implementar la ejecución de comandos y la redirección de entrada/salida, usando el código del ejercicio anterior.

## Ejercicio 4 - Traza de llamadas al sistema

El programa `trace.c` en _xv6_ activa o desactiva la traza de llamadas al sistema: imprime por la salida estándar las llamadas al sistema que son invocadas por los programas en ejecución. Al pasarle `1` como parámetro, activa la traza. Para desactivarla, se utiliza el parámetro `0`.

Por ejemplo, al ejecutar `trace 1` deberían tener una salida similar a la siguiente:
```console
$ trace 1
[27] sys_sctrace: 0
[3] sys_wait: 6
$[16] sys_write: 1
 [16] sys_write: 1
```

Luego, cuando ejecuten un programa, se imprimiran las llamadas al sistema que este invoque.

Responder:

1. Con la traza de llamadas al sistema activa, ejecutar el comando `echo hola`. Explicar por que se invoca cada una de las llamadas al sistema que se presentan en la traza.

## Ejercicio 5 - Implementar una llamada al sistema

En este ejercicio vamos a modificar el _kernel_ de _xv6_ para agregar una **nueva llamada al sistema** que retorne al usuario el número **42** (el sentido de la vida, el universo y todo lo demás).

Para realizar la implementación, usar como referencia como esta implementada la llamada al sistema `sys_trace`. Se deben modificar los siguientes archivos del sistema operativo:

- `usys.S`: implementa el mecanismo de invocación de una llamada al sistema desde el nivel de usuario.
- `user.h`: prototipos de las funciones de biblioteca para el usuario.
- `syscall.h`: identificadores de cada una de las llamadas al sistema.
- `syscall.c`: código que invoca la llamadas al sistema dentro del _kernel_.
- `sysproc.c`: aquí implementaremos la llamada al sistema, aunque podría estar en cualquier otro archivo `.c`.

El programa `answer.c` invoca la llamada al sistema e imprime el resultado (`42`). El código esta comentado dado que no existe la llamada. Una vez que la hayan implementado descomentar el código en el programa, recompilar y ejecutar nuevamente _xv6_ para verificar que se invoque correctamente la nueva llamada al sistema.

---

¡Fin del Laboratorio 2!

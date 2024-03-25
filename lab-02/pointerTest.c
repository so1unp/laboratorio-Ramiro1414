#include <stdio.h>

int main() {

    int x = 4;
    printf("x value: %d\n", x);

    int *pX = &x; // direccion de memoria que almacena el valor de x

    int y = *pX;
    printf("y value: %d\n", y);

    x -= 1;

    printf("x value: %d\n", x);
    printf("y value: %d\n", y);

    return 0;
}
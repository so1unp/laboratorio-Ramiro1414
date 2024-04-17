#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void catchSignal(int);

int main(void)
{
    struct sigaction handler;
    handler.sa_handler = catchSignal;

    printf("pid: %i\n", getpid());
    
    int i = 1;

    for(i = 1; i < 32; i++) {
        sigaction(i, &handler, NULL);
    }

    while( pause() ) {

    }   

    exit(EXIT_SUCCESS);
}

void catchSignal(int signum) {
    printf("%s\n", strsignal(signum));
}

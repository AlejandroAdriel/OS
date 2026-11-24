#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void handleSignal(int sig) {
    printf("[SIGNALIZACION] Proceso hijo capturo senal %d enviada por padre\n", sig);
}

int main(void) {
    // [TECNICA: FORK] Creacion de proceso hijo
    pid_t pid = fork();

    if (pid == 0) {
        // [TECNICA: SIGNALIZACION] Hijo registra manejador
        signal(SIGUSR1, handleSignal);
        pause(); // Espera la senal
        _exit(0);
    } else {
        sleep(1);
        // [TECNICA: SIGNALIZACION] Padre envia senal al PID del hijo
        kill(pid, SIGUSR1);
        // [TECNICA: FORK] Sincronizacion de proceso hijo
        wait(NULL);
    }
    return 0;
}

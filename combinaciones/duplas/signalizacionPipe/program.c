#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int pipefd[2];

// [TECNICA: SIGNALIZACION] Manejador que lee datos del pipe al capturar senal
void handleSignal(int sig) {
    char buf[32];
    // [TECNICA: PIPE] Lectura del extremo de lectura de la tuberia anonima
    int n = read(pipefd[0], buf, sizeof(buf) - 1);
    if (n > 0) buf[n] = '\0';
    printf("[SIGNALIZACION + PIPE] Senal %d capturada, leido de pipe: %s\n", sig, buf);
}

int main(void) {
    // [TECNICA: PIPE] Creacion de tuberia anonima
    pipe(pipefd);

    // [TECNICA: SIGNALIZACION] Registro de senal SIGUSR1
    signal(SIGUSR1, handleSignal);

    // [TECNICA: PIPE] Escritura en el extremo del pipe
    write(pipefd[1], "msg_pipe", 8);

    // [TECNICA: SIGNALIZACION] Disparo de la senal
    raise(SIGUSR1);

    close(pipefd[0]);
    close(pipefd[1]);
    return 0;
}

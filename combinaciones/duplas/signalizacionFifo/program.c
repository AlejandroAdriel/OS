#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_PATH "/tmp/dupla_sig_fifo"
int fd_fifo;

// [TECNICA: SIGNALIZACION] Manejador que recibe la senal y activa lectura
void handleSignal(int sig) {
    char buf[32];
    // [TECNICA: FIFO] Lectura desde la tuberia nombrada al recibir la senal
    int n = read(fd_fifo, buf, sizeof(buf) - 1);
    if (n > 0) buf[n] = '\0';
    printf("[SIGNALIZACION + FIFO] Senal %d recibida, leido de FIFO: %s\n", sig, buf);
}

int main(void) {
    unlink(FIFO_PATH);
    // [TECNICA: FIFO] Creacion de tuberia nombrada
    mkfifo(FIFO_PATH, 0666);

    // [TECNICA: SIGNALIZACION] Registro del manejador de senal
    signal(SIGUSR1, handleSignal);

    fd_fifo = open(FIFO_PATH, O_RDWR);
    // [TECNICA: FIFO] Escritura de datos en la FIFO
    write(fd_fifo, "datos_fifo", 10);

    // [TECNICA: SIGNALIZACION] Envio de senal para disparar el procesamiento
    raise(SIGUSR1);

    close(fd_fifo);
    unlink(FIFO_PATH);
    return 0;
}

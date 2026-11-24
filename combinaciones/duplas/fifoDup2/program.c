#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_PATH "/tmp/dupla_fifo_dup2"

int main(void) {
    char buf[32];
    unlink(FIFO_PATH);
    // [TECNICA: FIFO] Creacion de FIFO
    mkfifo(FIFO_PATH, 0666);

    int fdf = open(FIFO_PATH, O_RDWR);
    write(fdf, "entrada_redireccionada", 22);

    // [TECNICA: DUP2] Redireccion de entrada estandar (STDIN) para que lea de la FIFO
    dup2(fdf, STDIN_FILENO);
    close(fdf);

    // Lectura normal desde STDIN ahora consume de la FIFO
    int n = read(STDIN_FILENO, buf, sizeof(buf)-1);
    buf[n] = '\0';
    printf("[FIFO + DUP2] Leido desde STDIN (redireccionado a FIFO): %s\n", buf);

    unlink(FIFO_PATH);
    return 0;
}

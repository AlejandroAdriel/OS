#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_PATH "/tmp/dupla_fifo_pipe"

int main(void) {
    int pfd[2];
    char buf[32];
    unlink(FIFO_PATH);

    // [TECNICA: FIFO] Creacion de FIFO nombrada
    mkfifo(FIFO_PATH, 0666);
    // [TECNICA: PIPE] Creacion de pipe anonimo
    pipe(pfd);

    int fdf = open(FIFO_PATH, O_RDWR);
    // [TECNICA: FIFO] Escritura en FIFO
    write(fdf, "de_fifo", 7);

    // [TECNICA: FIFO] Lectura desde FIFO
    int n = read(fdf, buf, sizeof(buf)-1);
    buf[n] = '\0';

    // [TECNICA: PIPE] Reenvio a traves del pipe anonimo
    write(pfd[1], buf, n);
    close(pfd[1]);

    char buf2[32];
    // [TECNICA: PIPE] Lectura final desde el pipe
    int m = read(pfd[0], buf2, sizeof(buf2)-1);
    buf2[m] = '\0';
    printf("[FIFO + PIPE] Puente completado: %s\n", buf2);

    close(pfd[0]);
    close(fdf);
    unlink(FIFO_PATH);
    return 0;
}

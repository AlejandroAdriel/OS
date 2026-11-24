#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_PATH "/tmp/dupla_fifo_execv"

int main(void) {
    unlink(FIFO_PATH);
    // [TECNICA: FIFO] Preparacion de FIFO para nuevo binario
    mkfifo(FIFO_PATH, 0666);
    int fdf = open(FIFO_PATH, O_RDWR);
    write(fdf, "hola_execv", 10);
    close(fdf);

    printf("[FIFO + EXECV] FIFO lista. Ejecutando execv...\n");
    char *args[] = {"./hijo", NULL};
    // [TECNICA: EXECV] Reemplazo de memoria
    execv("./hijo", args);
    return 0;
}

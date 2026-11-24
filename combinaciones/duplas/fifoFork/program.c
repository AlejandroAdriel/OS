#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define FIFO_PATH "/tmp/dupla_fifo_fork"

int main(void) {
    unlink(FIFO_PATH);
    // [TECNICA: FIFO] Creacion de FIFO
    mkfifo(FIFO_PATH, 0666);

    // [TECNICA: FORK] Bifurcacion de procesos
    pid_t pid = fork();

    if (pid == 0) {
        // [TECNICA: FIFO] Hijo escribe en la FIFO
        int fd = open(FIFO_PATH, O_WRONLY);
        write(fd, "saludos_de_hijo", 15);
        close(fd);
        _exit(0);
    } else {
        // [TECNICA: FIFO] Padre lee de la FIFO
        int fd = open(FIFO_PATH, O_RDONLY);
        char buf[32];
        int n = read(fd, buf, sizeof(buf)-1);
        buf[n] = '\0';
        close(fd);
        // [TECNICA: FORK] Espera al hijo
        wait(NULL);
        printf("[FIFO + FORK] Padre recibio de hijo: %s\n", buf);
        unlink(FIFO_PATH);
    }
    return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <string.h>

#define FIFO_PATH "/tmp/dupla_fifo_shm"

int main(void) {
    unlink(FIFO_PATH);
    // [TECNICA: FIFO] Creacion de FIFO
    mkfifo(FIFO_PATH, 0666);

    // [TECNICA: SHARED MEMORY] Creacion de memoria compartida
    int shmid = shmget(IPC_PRIVATE, 64, 0666 | IPC_CREAT);
    char *shm = (char *)shmat(shmid, NULL, 0);

    // [TECNICA: FIFO] Envio de un token a traves de la FIFO
    int fdf = open(FIFO_PATH, O_RDWR);
    write(fdf, "token_ok", 8);

    char buf[16];
    read(fdf, buf, 8);
    // [TECNICA: SHARED MEMORY] Escritura en memoria compartida tras recibir token de FIFO
    strcpy(shm, "datos_en_shm_validados_por_fifo");

    printf("[FIFO + SHARED MEMORY] Shm contiene: %s\n", shm);

    close(fdf);
    unlink(FIFO_PATH);
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}

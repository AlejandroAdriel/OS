#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sem.h>

#define FIFO_PATH "/tmp/dupla_fifo_sem"

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main(void) {
    unlink(FIFO_PATH);
    // [TECNICA: FIFO] Creacion de FIFO
    mkfifo(FIFO_PATH, 0666);

    // [TECNICA: SEMAFORA] Creacion de semaforo de control
    int semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    union semun arg;
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);

    struct sembuf p = {0, -1, 0};
    struct sembuf v = {0, 1, 0};

    // [TECNICA: SEMAFORA] Bloqueo de acceso
    semop(semid, &p, 1);
    // [TECNICA: FIFO] Acceso exclusivo a la FIFO
    int fdf = open(FIFO_PATH, O_RDWR);
    write(fdf, "exclusivo_fifo", 14);
    char buf[32];
    int n = read(fdf, buf, sizeof(buf)-1);
    buf[n] = '\0';
    close(fdf);
    // [TECNICA: SEMAFORA] Desbloqueo
    semop(semid, &v, 1);

    printf("[FIFO + SEMAFORA] Operacion completada: %s\n", buf);

    unlink(FIFO_PATH);
    semctl(semid, 0, IPC_RMID, arg);
    return 0;
}

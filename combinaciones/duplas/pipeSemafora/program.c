#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main(void) {
    int pfd[2];
    // [TECNICA: PIPE] Creacion de pipe
    pipe(pfd);

    // [TECNICA: SEMAFORA] Creacion de semaforo de sincronizacion
    int semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    union semun arg;
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);

    struct sembuf p = {0, -1, 0};
    struct sembuf v = {0, 1, 0};

    // [TECNICA: SEMAFORA] Exclusion mutua (P)
    semop(semid, &p, 1);
    // [TECNICA: PIPE] Escritura protegida
    write(pfd[1], "pipe_con_semaforo", 17);
    close(pfd[1]);
    // [TECNICA: SEMAFORA] Liberacion (V)
    semop(semid, &v, 1);

    char buf[32];
    int n = read(pfd[0], buf, sizeof(buf)-1);
    buf[n] = '\0';
    close(pfd[0]);
    printf("[PIPE + SEMAFORA] Leido de pipe sincronizado: %s\n", buf);

    semctl(semid, 0, IPC_RMID, arg);
    return 0;
}

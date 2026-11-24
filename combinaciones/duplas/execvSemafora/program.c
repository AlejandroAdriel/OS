#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main(void) {
    // [TECNICA: SEMAFORA] Creacion de semaforo de sincronizacion
    int semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    union semun arg;
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);

    struct sembuf p = {0, -1, 0};
    // [TECNICA: SEMAFORA] Toma de semaforo
    semop(semid, &p, 1);

    printf("[EXECV + SEMAFORA] Semaforo tomado. Ejecutando execv...\n");
    char *args[] = {"./hijo", NULL};
    // [TECNICA: EXECV] Sustitucion de imagen
    execv("./hijo", args);
    return 0;
}

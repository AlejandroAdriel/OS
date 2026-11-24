#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main(void) {
    // [TECNICA: SHARED MEMORY] Creacion de segmento compartido
    int shmid = shmget(IPC_PRIVATE, 64, 0666 | IPC_CREAT);
    char *shm = (char *)shmat(shmid, NULL, 0);

    // [TECNICA: SEMAFORA] Creacion de semaforo para proteger la memoria compartida
    int semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    union semun arg;
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);

    struct sembuf p = {0, -1, 0};
    struct sembuf v = {0, 1, 0};

    // [TECNICA: SEMAFORA] Bloqueo P para exclusion mutua
    semop(semid, &p, 1);
    // [TECNICA: SHARED MEMORY] Escritura exclusiva
    strcpy(shm, "datos_protegidos_con_semaforo");
    printf("[SHARED MEMORY + SEMAFORA] Escrito en SHM: %s\n", shm);
    // [TECNICA: SEMAFORA] Desbloqueo V
    semop(semid, &v, 1);

    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID, arg);
    return 0;
}

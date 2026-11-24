#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main(void) {
    // [TECNICA: SEMAFORA] Creacion de semaforo
    int semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    union semun arg;
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);

    struct sembuf p = {0, -1, 0};
    struct sembuf v = {0, 1, 0};

    // [TECNICA: SEMAFORA] Exclusion mutua (P)
    semop(semid, &p, 1);

    // [TECNICA: DUP2] Manipulacion protegida de descriptores
    int clon = dup(STDOUT_FILENO);
    dup2(clon, STDOUT_FILENO);
    close(clon);

    // [TECNICA: SEMAFORA] Liberacion (V)
    semop(semid, &v, 1);

    printf("[DUP2 + SEMAFORA] Descriptor manipulado bajo exclusion de semaforo\n");

    semctl(semid, 0, IPC_RMID, arg);
    return 0;
}

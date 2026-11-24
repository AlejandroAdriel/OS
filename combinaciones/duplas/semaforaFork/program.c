#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/wait.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main(void) {
    // [TECNICA: SEMAFORA] Creacion de semaforo de sincronizacion binaria
    int semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    union semun arg;
    arg.val = 0; // Inicia en 0 (hijo se bloqueara hasta que el padre libere)
    semctl(semid, 0, SETVAL, arg);

    // [TECNICA: FORK] Bifurcacion
    pid_t pid = fork();

    if (pid == 0) {
        struct sembuf p = {0, -1, 0};
        // [TECNICA: SEMAFORA] Hijo espera con P
        semop(semid, &p, 1);
        printf("[SEMAFORA + FORK] Hijo fue liberado por el semaforo del padre\n");
        _exit(0);
    } else {
        sleep(1);
        struct sembuf v = {0, 1, 0};
        // [TECNICA: SEMAFORA] Padre despierta al hijo con V
        printf("[SEMAFORA + FORK] Padre enviando senal V via semaforo...\n");
        semop(semid, &v, 1);
        // [TECNICA: FORK] Espera final al hijo
        wait(NULL);
        semctl(semid, 0, IPC_RMID, arg);
    }
    return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/sem.h>

int semid;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

// [TECNICA: SIGNALIZACION] Manejador de senal
void handleSignal(int sig) {
    // [TECNICA: SEMAFORA] Liberacion / operacion V del semaforo
    struct sembuf v = {0, 1, 0};
    semop(semid, &v, 1);
    printf("[SIGNALIZACION + SEMAFORA] Senal %d recibida: semaforo liberado (V)\n", sig);
}

int main(void) {
    // [TECNICA: SEMAFORA] Creacion e inicializacion del semaforo
    semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    union semun arg;
    arg.val = 0;
    semctl(semid, 0, SETVAL, arg);

    // [TECNICA: SIGNALIZACION] Registro de senal
    signal(SIGUSR1, handleSignal);

    // [TECNICA: SIGNALIZACION] Disparo de senal
    raise(SIGUSR1);

    // [TECNICA: SEMAFORA] Operacion P (bloqueo) que fue habilitada por la senal
    struct sembuf p = {0, -1, 0};
    semop(semid, &p, 1);
    printf("[SEMAFORA] Paso exitoso por semaforo tras senal\n");

    semctl(semid, 0, IPC_RMID, arg);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM_KEY   0x5678
#define SEM_KEY   0x1111
#define SHMSZ     128
#define NUM_PROCS 6   // <-- Cambia este número a la cantidad de procesos deseada

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main(void) {
    // 1. Crear memoria compartida
    int shmid = shmget(SHM_KEY, SHMSZ, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    char *shm = (char *)shmat(shmid, NULL, 0);
    if (shm == (char *)-1) {
        perror("shmat");
        exit(1);
    }
    shm[0] = '\0';

    // 2. Crear conjunto de NUM_PROCS semáforos
    int semid = semget(SEM_KEY, NUM_PROCS, IPC_CREAT | 0666);
    if (semid < 0) {
        perror("semget");
        exit(1);
    }

    // Inicializar: el semáforo 0 en 1 (abierto); todos los demás en 0 (bloqueados)
    unsigned short valores[NUM_PROCS];
    valores[0] = 1;
    for (int i = 1; i < NUM_PROCS; i++) {
        valores[i] = 0;
    }

    union semun u;
    u.array = valores;
    if (semctl(semid, 0, SETALL, u) < 0) {
        perror("semctl SETALL");
        exit(1);
    }

    printf("[Servidor] SHM id: %d, SEM id: %d listos para %d procesos.\n", shmid, semid, NUM_PROCS);
    printf("[Servidor] Presiona ENTER cuando terminen los procesos para limpiar recursos...\n\n");
    getchar();

    printf("[Servidor] Último valor leído en memoria: \"%s\"\n", shm);

    // 3. Limpieza de recursos IPC
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    printf("[Servidor] Recursos IPC eliminados correctamente.\n");

    return 0;
}

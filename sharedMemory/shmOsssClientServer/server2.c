#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM_KEY 0x5678
#define SEM_KEY 0x1111
#define SHMSZ   128

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
    // Inicializar contenido vacío
    shm[0] = '\0';

    // 2. Crear conjunto de semáforo con 1 semáforo
    int semid = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    if (semid < 0) {
        perror("semget");
        exit(1);
    }

    // Inicializar el semáforo a 1 (recurso disponible / mutex abierto)
    union semun u;
    u.val = 1;
    if (semctl(semid, 0, SETVAL, u) < 0) {
        perror("semctl SETVAL");
        exit(1);
    }

    printf("[Servidor] SHM id: %d, SEM id: %d listos.\n", shmid, semid);
    printf("[Servidor] Presiona ENTER cuando terminen los 5 clientes para limpiar recursos...\n");
    getchar();

    printf("[Servidor] Último valor leído en memoria: \"%s\"\n", shm);

    // 3. Limpieza de recursos IPC
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    printf("[Servidor] Recursos IPC eliminados correctamente.\n");

    return 0;
}

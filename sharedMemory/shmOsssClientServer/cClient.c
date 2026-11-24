#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM_KEY   0x5678
#define SEM_KEY   0x1111
#define SHMSZ     128

// Operación P (-1) sobre el semáforo común
void semP(int semid) {
    struct sembuf op = {0, -1, 0};
    if (semop(semid, &op, 1) < 0) {
        perror("semop P");
        exit(13);
    }
}

// Operación V (+1) sobre el semáforo común
void semV(int semid) {
    struct sembuf op = {0, 1, 0};
    if (semop(semid, &op, 1) < 0) {
        perror("semop V");
        exit(14);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <ID_PROCESO> \"<MENSAJE>\"\n", argv[0]);
        exit(1);
    }

    int proc_id = atoi(argv[1]);
    char *mensaje = argv[2];

    // Conectar a memoria compartida existente
    int shmid = shmget(SHM_KEY, SHMSZ, 0666);
    if (shmid < 0) {
        perror("shmget cliente");
        exit(1);
    }

    char *shm = (char *)shmat(shmid, NULL, 0);
    if (shm == (char *)-1) {
        perror("shmat cliente");
        exit(1);
    }

    // Conectar al semáforo existente (solo 1)
    int semid = semget(SEM_KEY, 1, 0666);
    if (semid < 0) {
        perror("semget cliente");
        exit(1);
    }

    printf("[P%d] Esperando semáforo para entrar a memoria compartida...\n", proc_id);

    // 1. Competencia libre (entra el primero que gane el semáforo)
    semP(semid);

    // 2. SECCIÓN CRÍTICA: Exclusión mutua garantizada
    printf("[P%d] >>> DENTRO de la memoria. Leyó: \"%s\"\n", proc_id, shm);

    snprintf(shm, SHMSZ, "%s", mensaje);
    sleep(1); // Simula el tiempo de procesamiento

    printf("[P%d] <<< Saliendo y liberando semáforo.\n\n", proc_id);

    // 3. Liberar el semáforo para el siguiente proceso en espera
    semV(semid);

    shmdt(shm);
    return 0;
}

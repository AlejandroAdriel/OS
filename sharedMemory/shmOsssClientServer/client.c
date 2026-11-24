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
#define NUM_PROCS 6   // <-- Debe coincidir con el valor definido en server.c

// Operación P (-1) sobre un semáforo específico
void semP(int semid, int sem_num) {
    struct sembuf op = {(unsigned short)sem_num, -1, 0};
    if (semop(semid, &op, 1) < 0) {
        perror("semop P");
        exit(13);
    }
}

// Operación V (+1) sobre un semáforo específico
void semV(int semid, int sem_num) {
    struct sembuf op = {(unsigned short)sem_num, 1, 0};
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

    // Validación de ID válido respecto a NUM_PROCS
    if (proc_id < 1 || proc_id > NUM_PROCS) {
        fprintf(stderr, "Error: ID_PROCESO debe estar entre 1 y %d\n", NUM_PROCS);
        exit(1);
    }

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

    // Conectar al conjunto de semáforos existente
    int semid = semget(SEM_KEY, NUM_PROCS, 0666);
    if (semid < 0) {
        perror("semget cliente");
        exit(1);
    }

    int mi_sem = proc_id - 1;   // Proceso 1 espera sem 0, Proceso 2 espera sem 1, etc.
    int sig_sem = proc_id;      // Siguiente semáforo a despertar

    // 1. Espera de mi turno exclusivo
    semP(semid, mi_sem);

    // 2. SECCIÓN CRÍTICA
    printf("[P%d] >>> DENTRO de la memoria. Leyó: \"%s\"\n", proc_id, shm);

    snprintf(shm, SHMSZ, "%s", mensaje);
    sleep(1);

    printf("[P%d] <<< Saliendo y habilitando siguiente proceso.\n", proc_id);

    // 3. Despertar al siguiente proceso si no es el último
    if (sig_sem < NUM_PROCS) {
        semV(semid, sig_sem);
    }

    shmdt(shm);
    return 0;
}
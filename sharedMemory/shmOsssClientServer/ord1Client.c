#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM_KEY 0x5678
#define SEM_KEY 0x1111
#define SHMSZ   128

// Operación P (-1) sobre un semáforo específico del conjunto
void semP(int semid, int sem_num) {
    struct sembuf op = {(unsigned short)sem_num, -1, 0};
    if (semop(semid, &op, 1) < 0) {
        perror("semop P");
        exit(13);
    }
}

// Operación V (+1) sobre un semáforo específico del conjunto
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

    // Conectar a la memoria compartida
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

    // Conectar al conjunto de 5 semáforos
    int semid = semget(SEM_KEY, 5, 0666);
    if (semid < 0) {
        perror("semget cliente");
        exit(1);
    }

    int mi_sem = proc_id - 1;   // Proceso 1 espera al semáforo 0, Proceso 2 al 1, etc.
    int sig_sem = proc_id;      // Siguiente semáforo a desbloquear

    // 1. Espera atómica controlada por el kernel (sin bucle while ni sleep innecesario)
    semP(semid, mi_sem);

    // 2. SECCIÓN CRÍTICA: Acceso exclusivo a shm
    printf("[P%d] >>> DENTRO de la memoria. Leyó: \"%s\"\n", proc_id, shm);

    snprintf(shm, SHMSZ, "%s", mensaje);
    sleep(1); // Simula trabajo

    printf("[P%d] <<< Saliendo y habilitando siguiente proceso.\n", proc_id);

    // 3. Despertar al siguiente proceso si no es el último
    if (sig_sem < 5) {
        semV(semid, sig_sem);
    }

    shmdt(shm);
    return 0;
}

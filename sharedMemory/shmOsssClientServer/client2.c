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

struct sembuf p = {0, -1, 0};
struct sembuf v = {0, +1, 0};

void semP(int id) {
    if (semop(id, &p, 1) < 0) {
        perror("semop P");
        exit(13);
    }
}

void semV(int id) {
    if (semop(id, &v, 1) < 0) {
        perror("semop V");
        exit(14);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <ID_PROCESO>\n", argv[0]);
        exit(1);
    }

    int proc_id = atoi(argv[1]);

    // 1. Obtener la memoria compartida existente
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

    // 2. Obtener el semáforo existente
    int semid = semget(SEM_KEY, 1, 0666);
    if (semid < 0) {
        perror("semget cliente");
        exit(1);
    }

    printf("[P%d] Esperando semáforo para entrar a memoria compartida...\n", proc_id);
    semP(semid); // <--- ENTRADA A SECCIÓN CRÍTICA

    printf("[P%d] >>> DENTRO de la memoria. Leyó: \"%s\"\n", proc_id, shm);
    
    // Escribir en memoria compartida
    snprintf(shm, SHMSZ, "Mensaje escrito por proceso %d", proc_id);
    
    // Simular procesamiento para evidenciar que nadie más entra simultáneamente
    sleep(2);
    
    printf("[P%d] <<< Saliendo y liberando semáforo.\n", proc_id);
    semV(semid); // <--- SALIDA DE SECCIÓN CRÍTICA

    shmdt(shm);
    return 0;
}

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
    // 1. Verificamos que se pasen el ID y el mensaje personalizado
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <ID_PROCESO> \"<MENSAJE>\"\n", argv[0]);
        exit(1);
    }

    int proc_id = atoi(argv[1]);
    char *mensaje = argv[2]; // Capturado ANTES de entrar a la sección crítica

    // 2. Conexión a memoria compartida y semáforo existentes
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

    int semid = semget(SEM_KEY, 1, 0666);
    if (semid < 0) {
        perror("semget cliente");
        exit(1);
    }

    printf("[P%d] Esperando semáforo para entrar a memoria compartida...\n", proc_id);
    semP(semid); // <--- ENTRADA A SECCIÓN CRÍTICA

    printf("[P%d] >>> DENTRO de la memoria. Leyó: \"%s\"\n", proc_id, shm);
    
    // Escribir el mensaje personalizado que vino por terminal
    snprintf(shm, SHMSZ, "%s", mensaje);
    
    // Pausa breve para simular trabajo y comprobar exclusión mutua
    sleep(2);
    
    printf("[P%d] <<< Saliendo y liberando semáforo.\n", proc_id);
    semV(semid); // <--- SALIDA DE SECCIÓN CRÍTICA

    shmdt(shm);
    return 0;
}

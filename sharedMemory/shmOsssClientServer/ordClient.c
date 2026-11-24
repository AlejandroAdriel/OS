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
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <ID_PROCESO> \"<MENSAJE>\"\n", argv[0]);
        exit(1);
    }

    int proc_id = atoi(argv[1]);
    char mi_turno = '0' + proc_id; // Si proc_id es 1, mi_turno es '1'
    char *mensaje = argv[2];

    int shmid = shmget(SHM_KEY, SHMSZ, 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    char *shm = (char *)shmat(shmid, NULL, 0);
    if (shm == (char *)-1) {
        perror("shmat");
        exit(1);
    }

    int semid = semget(SEM_KEY, 1, 0666);
    if (semid < 0) {
        perror("semget");
        exit(1);
    }

    // Bucle para esperar mi turno
    while (1) {
        semP(semid); // Cerrojo para revisar el primer byte de forma segura

        if (shm[0] == mi_turno) {
            // Seccion critica
            printf("[P%d] >>> DENTRO. Leyo: \"%s\"\n", proc_id, &shm[1]);

            // Copiar el mensaje a partir de la posicion 1
            snprintf(&shm[1], SHMSZ - 1, "%s", mensaje);
            sleep(1);

            printf("[P%d] <<< Saliendo y pasando turno al siguiente.\n", proc_id);
            shm[0]++; // Pasa el turno al siguiente caracter ('1' -> '2' -> '3'...)

            semV(semid); // Soltar cerrojo
            break;
        }

        // Si no era mi turno, suelto el cerrojo y duermo 50ms antes de reintentar
        semV(semid);
        usleep(50000);
    }

    shmdt(shm);
    return 0;
}

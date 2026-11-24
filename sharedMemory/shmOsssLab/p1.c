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
#define SHMSZ     2000

void semP(int id) {
    struct sembuf op = {0, -1, 0};
    semop(id, &op, 1);
}

void semV(int id) {
    struct sembuf op = {0, 1, 0};
    semop(id, &op, 1);
}

int main(void) {
    int shmid = shmget(SHM_KEY, SHMSZ, 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    int id = semget(SEM_KEY, 2, 0666);
    if (id < 0) {
        perror("semget");
        exit(1);
    }

    char *shm = (char *)shmat(shmid, NULL, 0);
    if (shm == (char *)-1) {
        perror("shmat");
        exit(1);
    }


    for (int i = 0; i <= 100; i += 2) {
        semP(id); 

        printf("[P1] Lectura actual: \"%s\"\n", shm);

        // Concatenar número par
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%d ", i);
        strcat(shm, buffer);

        printf("[P1] Escribio: %d\n\n", i);

        semV(id); 
    }

    shmdt(shm);
    return 0;
}
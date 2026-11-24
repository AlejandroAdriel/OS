#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <string.h>

int main(void) {
    // [TECNICA: SHARED MEMORY] Creacion de memoria compartida
    int shmid = shmget(IPC_PRIVATE, 64, 0666 | IPC_CREAT);
    char *shm = (char *)shmat(shmid, NULL, 0);
    strcpy(shm, "datos_validados_shm");

    // [TECNICA: DUP2] Operacion sobre tabla de descriptores
    int clon = dup(STDOUT_FILENO);
    dup2(clon, STDOUT_FILENO);
    close(clon);

    // [TECNICA: SHARED MEMORY] Uso conjunto
    printf("[DUP2 + SHARED MEMORY] Accediendo a SHM tras dup2: %s\n", shm);

    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}

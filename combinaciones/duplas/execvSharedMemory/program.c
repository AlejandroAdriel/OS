#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <string.h>

int main(void) {
    // [TECNICA: SHARED MEMORY] Creacion de segmento compartido
    int shmid = shmget(IPC_PRIVATE, 64, 0666 | IPC_CREAT);
    char *shm = (char *)shmat(shmid, NULL, 0);
    strcpy(shm, "datos_persistentes_en_shm");

    printf("[EXECV + SHARED MEMORY] Shm lista (shmid=%d). Ejecutando execv...\n", shmid);
    char *args[] = {"./hijo", NULL};
    // [TECNICA: EXECV] Sustitucion de proceso (shm persiste en el kernel)
    execv("./hijo", args);
    return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <string.h>

int main(void) {
    // [TECNICA: SHARED MEMORY] Creacion de segmento previo a fork
    int shmid = shmget(IPC_PRIVATE, 64, 0666 | IPC_CREAT);
    char *shm = (char *)shmat(shmid, NULL, 0);

    // [TECNICA: FORK] Bifurcacion de procesos (ambos heredan puntero/attachment)
    pid_t pid = fork();

    if (pid == 0) {
        // [TECNICA: SHARED MEMORY] Hijo escribe en memoria compartida
        strcpy(shm, "mensaje_del_hijo_por_shm");
        shmdt(shm);
        _exit(0);
    } else {
        // [TECNICA: FORK] Padre espera a que hijo termine de escribir
        wait(NULL);
        // [TECNICA: SHARED MEMORY] Padre lee directamente lo escrito por el hijo
        printf("[SHARED MEMORY + FORK] Padre leyo de SHM: %s\n", shm);
        shmdt(shm);
        shmctl(shmid, IPC_RMID, NULL);
    }
    return 0;
}

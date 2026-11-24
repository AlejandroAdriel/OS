#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>
#include <string.h>

char *shm_ptr;

// [TECNICA: SIGNALIZACION] Manejador de senal
void handleSignal(int sig) {
    // [TECNICA: SHARED MEMORY] Lectura directa de la memoria compartida
    printf("[SIGNALIZACION + SHARED MEMORY] Senal %d recibida. Shm contorno: %s\n", sig, shm_ptr);
}

int main(void) {
    // [TECNICA: SHARED MEMORY] Creacion y attachment de memoria compartida
    int shmid = shmget(IPC_PRIVATE, 64, 0666 | IPC_CREAT);
    shm_ptr = (char *)shmat(shmid, NULL, 0);

    // [TECNICA: SIGNALIZACION] Registro del manejador
    signal(SIGUSR1, handleSignal);

    // [TECNICA: SHARED MEMORY] Escritura en memoria compartida
    strcpy(shm_ptr, "mensaje_en_shm");

    // [TECNICA: SIGNALIZACION] Disparo de senal
    raise(SIGUSR1);

    // [TECNICA: SHARED MEMORY] Desconexion y eliminacion
    shmdt(shm_ptr);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}

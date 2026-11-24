#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <string.h>

int main(void) {
    int pfd[2];
    // [TECNICA: PIPE] Creacion de pipe
    pipe(pfd);

    // [TECNICA: SHARED MEMORY] Creacion de memoria compartida
    int shmid = shmget(IPC_PRIVATE, 64, 0666 | IPC_CREAT);
    char *shm = (char *)shmat(shmid, NULL, 0);

    // [TECNICA: SHARED MEMORY] Almacenamiento previo
    strcpy(shm, "datos_shm_para_pipe");

    // [TECNICA: PIPE] Escritura del puntero/datos al pipe
    write(pfd[1], shm, strlen(shm));
    close(pfd[1]);

    char buf[64];
    // [TECNICA: PIPE] Lectura desde el pipe
    int n = read(pfd[0], buf, sizeof(buf)-1);
    buf[n] = '\0';
    close(pfd[0]);

    printf("[PIPE + SHARED MEMORY] Dato transferido desde shm via pipe: %s\n", buf);

    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}

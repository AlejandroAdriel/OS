#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <string.h>

struct msgbuf {
    long mtype;
    int shmid; // Pasamos el ID de la memoria compartida por la cola de mensajes
};

int main(void) {
    // [TECNICA: SHARED MEMORY] Segmento creado con datos
    int shmid = shmget(IPC_PRIVATE, 64, 0666 | IPC_CREAT);
    char *shm = (char *)shmat(shmid, NULL, 0);
    strcpy(shm, "datos_pesados_en_shm");

    // [TECNICA: MESSAGE PASSING] Cola para senalizar el ID de la SHM
    int msqid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    struct msgbuf msg = {1, shmid};
    msgsnd(msqid, &msg, sizeof(msg.shmid), 0);

    struct msgbuf rec;
    msgrcv(msqid, &rec, sizeof(rec.shmid), 1, 0);

    // [TECNICA: SHARED MEMORY] Receptor enlaza al shmid recibido de la cola
    char *shm_rec = (char *)shmat(rec.shmid, NULL, 0);
    printf("[MESSAGE PASSING + SHARED MEMORY] Shmid %d recibido por cola. Contenido SHM: %s\n", rec.shmid, shm_rec);

    shmdt(shm);
    shmdt(shm_rec);
    shmctl(shmid, IPC_RMID, NULL);
    msgctl(msqid, IPC_RMID, NULL);
    return 0;
}

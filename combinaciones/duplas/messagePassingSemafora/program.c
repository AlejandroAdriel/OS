#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/sem.h>

struct msgbuf {
    long mtype;
    char mtext[32];
};

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main(void) {
    // [TECNICA: MESSAGE PASSING] Creacion de cola
    int msqid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

    // [TECNICA: SEMAFORA] Semaforo de conteo
    int semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    union semun arg;
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);

    struct sembuf p = {0, -1, 0};
    struct sembuf v = {0, 1, 0};

    // [TECNICA: SEMAFORA] Seccion critica con P
    semop(semid, &p, 1);
    struct msgbuf msg = {1, "msg_sincronizado"};
    // [TECNICA: MESSAGE PASSING] Envio
    msgsnd(msqid, &msg, sizeof(msg.mtext), 0);
    // [TECNICA: SEMAFORA] Liberacion V
    semop(semid, &v, 1);

    struct msgbuf rec;
    msgrcv(msqid, &rec, sizeof(rec.mtext), 1, 0);
    printf("[MESSAGE PASSING + SEMAFORA] Mensaje extraido: %s\n", rec.mtext);

    msgctl(msqid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID, arg);
    return 0;
}

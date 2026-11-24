#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>

struct msgbuf {
    long mtype;
    char mtext[32];
};

int main(void) {
    int pfd[2];
    // [TECNICA: PIPE] Creacion de tuberia anonima
    pipe(pfd);
    write(pfd[1], "puente_pipe", 11);
    close(pfd[1]);

    // [TECNICA: MESSAGE PASSING] Creacion de cola IPC
    int msqid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

    char buf[32];
    // [TECNICA: PIPE] Lectura de pipe
    int n = read(pfd[0], buf, sizeof(buf)-1);
    buf[n] = '\0';
    close(pfd[0]);

    // [TECNICA: MESSAGE PASSING] Transferencia a la cola de mensajes
    struct msgbuf msg = {1, ""};
    sprintf(msg.mtext, "%s_a_cola", buf);
    msgsnd(msqid, &msg, sizeof(msg.mtext), 0);

    struct msgbuf rec;
    msgrcv(msqid, &rec, sizeof(rec.mtext), 1, 0);
    printf("[PIPE + MESSAGE PASSING] Mensaje recibido de la cola: %s\n", rec.mtext);

    msgctl(msqid, IPC_RMID, NULL);
    return 0;
}

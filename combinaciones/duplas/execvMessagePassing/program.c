#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>

struct msgbuf {
    long mtype;
    char mtext[32];
};

int main(void) {
    // [TECNICA: MESSAGE PASSING] Creacion y poblado de cola antes de cambiar de ejecutable
    int msqid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    struct msgbuf msg = {1, "msg_previo_a_execv"};
    msgsnd(msqid, &msg, sizeof(msg.mtext), 0);

    printf("[EXECV + MESSAGE PASSING] Cola lista (msqid=%d). Reemplazando con execv...\n", msqid);
    char *args[] = {"./hijo", NULL};
    // [TECNICA: EXECV] Sustitucion de ejecutable
    execv("./hijo", args);
    return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>

struct msgbuf {
    long mtype;
    char mtext[32];
};

int main(void) {
    // [TECNICA: MESSAGE PASSING] Creacion de cola IPC
    int msqid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    struct msgbuf msg = {1, "mensaje_para_dup2"};
    msgsnd(msqid, &msg, sizeof(msg.mtext), 0);

    // [TECNICA: DUP2] Clonacion de descriptor STDIN sobre STDIN (validacion de descriptor)
    int clon_in = dup(STDIN_FILENO);
    dup2(clon_in, STDIN_FILENO);
    close(clon_in);

    struct msgbuf rec;
    // [TECNICA: MESSAGE PASSING] Recepcion
    msgrcv(msqid, &rec, sizeof(rec.mtext), 1, 0);
    printf("[DUP2 + MESSAGE PASSING] Recibido: %s tras clonacion dup2\n", rec.mtext);

    msgctl(msqid, IPC_RMID, NULL);
    return 0;
}

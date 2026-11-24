#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/wait.h>

struct msgbuf {
    long mtype;
    char mtext[32];
};

int main(void) {
    // [TECNICA: MESSAGE PASSING] Creacion de cola previa a fork
    int msqid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

    // [TECNICA: FORK] Bifurcacion
    pid_t pid = fork();

    if (pid == 0) {
        // [TECNICA: MESSAGE PASSING] Hijo envia mensaje a la cola
        struct msgbuf msg = {1, "hola_padre_desde_cola"};
        msgsnd(msqid, &msg, sizeof(msg.mtext), 0);
        _exit(0);
    } else {
        struct msgbuf rec;
        // [TECNICA: MESSAGE PASSING] Padre recibe el mensaje del hijo
        msgrcv(msqid, &rec, sizeof(rec.mtext), 1, 0);
        printf("[MESSAGE PASSING + FORK] Padre extrajo: %s\n", rec.mtext);
        // [TECNICA: FORK] Espera al hijo
        wait(NULL);
        msgctl(msqid, IPC_RMID, NULL);
    }
    return 0;
}

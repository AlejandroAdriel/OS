#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/msg.h>

struct msgbuf {
    long mtype;
    char mtext[32];
};

int msqid;

// [TECNICA: SIGNALIZACION] Manejador que extrae mensaje de la cola al recibir senal
void handleSignal(int sig) {
    struct msgbuf msg;
    // [TECNICA: MESSAGE PASSING] Recepcion desde la cola de mensajes System V
    msgrcv(msqid, &msg, sizeof(msg.mtext), 1, 0);
    printf("[SIGNALIZACION + MESSAGE PASSING] Senal %d: Mensaje IPC leido: %s\n", sig, msg.mtext);
}

int main(void) {
    // [TECNICA: MESSAGE PASSING] Creacion de cola de mensajes
    msqid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

    // [TECNICA: SIGNALIZACION] Registro de senal
    signal(SIGUSR1, handleSignal);

    struct msgbuf msg = {1, "hola_cola_ipc"};
    // [TECNICA: MESSAGE PASSING] Envio a la cola de mensajes
    msgsnd(msqid, &msg, sizeof(msg.mtext), 0);

    // [TECNICA: SIGNALIZACION] Envio de senal
    raise(SIGUSR1);

    // [TECNICA: MESSAGE PASSING] Eliminacion de la cola
    msgctl(msqid, IPC_RMID, NULL);
    return 0;
}

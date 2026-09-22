#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define KEY_MQ2 5678
#define MAXSIZE 128

struct msgbuf {
    long mtype;
    char mtext[MAXSIZE];
};

int main(void) {
    int msqid2 = msgget(KEY_MQ2, IPC_CREAT | 0666);
    if (msqid2 < 0) {
        perror("Error obteniendo MQ2");
        exit(1);
    }

    printf("Program 3, PID: %d\n", getpid());
    printf("Esperando mensaje Tipo 4 desde MQ2...\n");

    while (1) {
        struct msgbuf rbuf;

        if (msgrcv(msqid2, &rbuf, sizeof(rbuf.mtext), 4, 0) > 0) {
            printf("\nProgram 3: Message received, type 4\n");
            printf("Program 3: Message displayed -> \"%s\"\n", rbuf.mtext);
        } else {
            perror("Error al recibir mensaje en MQ2");
        }
    }

    return 0;
}

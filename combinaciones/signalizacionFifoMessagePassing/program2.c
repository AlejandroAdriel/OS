#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define KEY_MQ1 1234
#define KEY_MQ2 5678
#define MAXSIZE 128

struct msgbuf {
    long mtype;
    char mtext[MAXSIZE];
};

int main(void) {
    int msqid1 = msgget(KEY_MQ1, IPC_CREAT | 0666);
    int msqid2 = msgget(KEY_MQ2, IPC_CREAT | 0666);

    if (msqid1 < 0 || msqid2 < 0) {
        perror("Error obteniendo colas IPC");
        exit(1);
    }

    printf("Program 2, PID: %d \n", getpid());
    printf("Esperando mensaje Tipo 3 desde MQ1...\n");

    while (1) {
        struct msgbuf rbuf, sbuf;

        if (msgrcv(msqid1, &rbuf, sizeof(rbuf.mtext), 3, 0) > 0) {
            printf("\nProgram 2: Message received, type 3 -> \"%s\"\n", rbuf.mtext);

            sbuf.mtype = 4; 
            snprintf(sbuf.mtext, sizeof(sbuf.mtext), "%s", rbuf.mtext);

            if (msgsnd(msqid2, &sbuf, strlen(sbuf.mtext) + 1, 0) < 0) {
                perror("Error enviando a MQ2");
            } else {
                printf("Program 2: Message sent, type 4 -> \"%s\"\n", sbuf.mtext);
            }
        }
    }

    return 0;
}

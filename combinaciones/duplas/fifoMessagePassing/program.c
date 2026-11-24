#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/msg.h>

#define FIFO_PATH "/tmp/dupla_fifo_mq"

struct msgbuf {
    long mtype;
    char mtext[32];
};

int main(void) {
    unlink(FIFO_PATH);
    // [TECNICA: FIFO] Creacion de FIFO
    mkfifo(FIFO_PATH, 0666);
    int fdf = open(FIFO_PATH, O_RDWR);
    write(fdf, "datos_en_fifo", 13);

    // [TECNICA: MESSAGE PASSING] Creacion de cola de mensajes
    int msqid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

    char buf[32];
    int n = read(fdf, buf, sizeof(buf)-1);
    buf[n] = '\0';

    // [TECNICA: MESSAGE PASSING] Envio del dato leido de la FIFO a la cola IPC
    struct msgbuf msg = {1, ""};
    sprintf(msg.mtext, "%s", buf);
    msgsnd(msqid, &msg, sizeof(msg.mtext), 0);

    struct msgbuf rec;
    msgrcv(msqid, &rec, sizeof(rec.mtext), 1, 0);
    printf("[FIFO + MESSAGE PASSING] Recibido de la cola IPC (origen FIFO): %s\n", rec.mtext);

    close(fdf);
    unlink(FIFO_PATH);
    msgctl(msqid, IPC_RMID, NULL);
    return 0;
}

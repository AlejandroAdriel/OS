#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXSIZE 128

void die(char *s)
{
    perror(s);
    exit(1);
}

struct msgbuf
{
    long mtype;
    char mtext[MAXSIZE];
};

int main(void)
{
    int msqid;
    key_t key;
    struct msgbuf rcvbuffer;
    
    key = 1234;

    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0)
        die("msgget()");

    printf("Proceso receptor iniciado.\n");
    printf("Esperando mensajes en cola IPC (Key: %d, ID: %d)...\n", key, msqid);

    while (1) {
        // msgrcv con msgtyp = 0 recibe cualquier mensaje en orden FIFO
        if (msgrcv(msqid, &rcvbuffer, sizeof(rcvbuffer.mtext), 0, 0) < 0)
            die("msgrcv");

        printf("[Mensaje Recibido]\n");
        printf("  Tipo: %ld\n", rcvbuffer.mtype);
        printf("  Contenido: %s\n", rcvbuffer.mtext);
        printf("  Procesando mensaje (espera de 5s)...\n");

        sleep(5);

        printf("  Procesamiento completado. Esperando siguiente mensaje...\n\n");
    }

    return 0;
}

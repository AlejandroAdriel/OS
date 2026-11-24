#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
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

static sigjmp_buf salto_sigint;
static sigjmp_buf salto_sigusr1;
static sigjmp_buf salto_sigusr2;

void handler_sigint(int signo)
{
    (void)signo;
    printf("\nSe recibió la señal SIGINT (Tipo 1)\n");
    siglongjmp(salto_sigint, 1);
}

void handler_sigusr1(int signo)
{
    (void)signo;
    printf("\nSe recibió la señal SIGUSR1 (Tipo 2)\n");
    siglongjmp(salto_sigusr1, 1);
}

void handler_sigusr2(int signo)
{
    (void)signo;
    printf("\nSe recibió la señal SIGUSR2 (Tipo 3)\n");
    siglongjmp(salto_sigusr2, 1);
}

int main(void)
{
    int msqid;
    int msgflg = IPC_CREAT | 0666;
    key_t key;
    struct msgbuf sbuf;
    size_t buflen;
    
    key = 1234;

    if ((msqid = msgget(key, msgflg)) < 0)
        die("msgget");

    signal(SIGINT, handler_sigint);
    signal(SIGUSR1, handler_sigusr1);
    signal(SIGUSR2, handler_sigusr2);

    printf("PID del proceso emisor: %d\n", getpid());
    printf("  Tipo 1: Ctrl+C  o  kill -2 %d (SIGINT)\n", getpid());
    printf("  Tipo 2: kill -10 %d          (SIGUSR1)\n", getpid());
    printf("  Tipo 3: kill -12 %d          (SIGUSR2)\n", getpid());
    printf("Esperando señales...\n");


    while (1) {
        if (sigsetjmp(salto_sigint, 1) != 0) {
            printf("¡Se realizó el salto de código (SIGINT)!\n");
            sbuf.mtype = 1;
            snprintf(sbuf.mtext, sizeof(sbuf.mtext), "Mensaje generado por SIGINT");
            buflen = strlen(sbuf.mtext) + 1;
            if (msgsnd(msqid, &sbuf, buflen, IPC_NOWAIT) < 0)
                die("msgsnd");
            printf("Mensaje tipo 1 enviado a la cola IPC. Esperando nuevas señales...\n\n");
        }

        if (sigsetjmp(salto_sigusr1, 1) != 0) {
            printf("¡Se realizó el salto de código (SIGUSR1)!\n");
            sbuf.mtype = 2;
            snprintf(sbuf.mtext, sizeof(sbuf.mtext), "Mensaje generado por SIGUSR1");
            buflen = strlen(sbuf.mtext) + 1;
            if (msgsnd(msqid, &sbuf, buflen, IPC_NOWAIT) < 0)
                die("msgsnd");
            printf("Mensaje tipo 2 enviado a la cola IPC. Esperando nuevas señales...\n\n");
        }

        if (sigsetjmp(salto_sigusr2, 1) != 0) {
            printf("¡Se realizó el salto de código (SIGUSR2)!\n");
            sbuf.mtype = 3;
            snprintf(sbuf.mtext, sizeof(sbuf.mtext), "Mensaje generado por SIGUSR2");
            buflen = strlen(sbuf.mtext) + 1;
            if (msgsnd(msqid, &sbuf, buflen, IPC_NOWAIT) < 0)
                die("msgsnd");
            printf("Mensaje tipo 3 enviado a la cola IPC. Esperando nuevas señales...\n\n");
        }

        pause();
    }

    return 0;
}

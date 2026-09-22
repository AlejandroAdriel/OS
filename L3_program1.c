#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define FIFO_PATH "/tmp/fifoLab"
#define KEY_MQ1 1234
#define MAXSIZE 128

struct msgbuf {
    long mtype;
    char mtext[MAXSIZE];
};

static sigjmp_buf salto_sigint;
static sigjmp_buf salto_sigusr1;

void handler_sigint(int signo) {
    (void)signo;
    siglongjmp(salto_sigint, 1);
}

void handler_sigusr1(int signo) {
    (void)signo;
    siglongjmp(salto_sigusr1, 1);
}

int main(int argc, char *argv[]) {
    int fd;
    char buf[MAXSIZE];
    snprintf(buf, sizeof(buf), "%s", (argc > 1) ? argv[1] : "jajajaja");

    int msqid1 = msgget(KEY_MQ1, IPC_CREAT | 0666);
    if (msqid1 < 0) {
        perror("Error en msgget MQ1");
        exit(1);
    }

    mkfifo(FIFO_PATH, 0666);

    signal(SIGINT, handler_sigint);  
    signal(SIGUSR1, handler_sigusr1); 

    printf("Program 1, PID: %d \n", getpid());
    printf("Mensaje inicial guardado: \"%s\"\n", buf);
    printf(" - Leer FIFO (Signal 2):   kill -2 %d\n - Enviar MQ (Signal 10):  kill -10 %d\n\n", getpid(), getpid());

    while (1) {
        if (sigsetjmp(salto_sigint, 1) != 0) {
            printf("\nProgram 1: Signal 2 received\n");
            fd = open(FIFO_PATH, O_RDONLY);
            if (fd != -1) {
                ssize_t bytes = read(fd, buf, sizeof(buf) - 1);
                if (bytes > 0) {
                    buf[bytes] = '\0';
                    buf[strcspn(buf, "\r\n")] = 0; 
                    printf("Program 1: Data read from FIFO -> \"%s\"\n", buf);
                } else {
                    printf("FIFO vacio. Manteniendo datos previos: \"%s\"\n", buf);
                }
                close(fd);
            } else {
                perror("Error al abrir FIFO");
            }
        }

        if (sigsetjmp(salto_sigusr1, 1) != 0) {
            printf("\nProgram 1: Signal 10 received\n");
            struct msgbuf sbuf;
            sbuf.mtype = 3; 
            snprintf(sbuf.mtext, sizeof(sbuf.mtext), "%s", buf);

            if (msgsnd(msqid1, &sbuf, strlen(sbuf.mtext) + 1, IPC_NOWAIT) < 0) {
                perror("Error enviando a MQ1");
            } else {
                printf("Program 1: Message sent, type 3 -> \"%s\"\n", sbuf.mtext);
            }
        }

        pause();
    }

    return 0;
}

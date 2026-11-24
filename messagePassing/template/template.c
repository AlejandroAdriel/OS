#include <sys/msg.h>
#include <unistd.h>

struct msgbuf {
    long mtype;
    char mtext[1];
};

int main(void) {
    int msqid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    if (fork() == 0) {
        struct msgbuf msg;
        msgrcv(msqid, &msg, sizeof(msg.mtext), 1, 0);
        _exit(0);
    }
    struct msgbuf msg = {1, "a"};
    msgsnd(msqid, &msg, sizeof(msg.mtext), 0);
    msgctl(msqid, IPC_RMID, NULL);
    return 0;
}

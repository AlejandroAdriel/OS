#include <sys/shm.h>
#include <unistd.h>

int main(void) {
    int shmid = shmget(IPC_PRIVATE, 32, 0666 | IPC_CREAT);
    char *shm = (char *)shmat(shmid, NULL, 0);
    if (fork() == 0) {
        *shm = 'x';
        shmdt(shm);
        _exit(0);
    }
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}

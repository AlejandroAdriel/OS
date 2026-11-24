#include <unistd.h>

int main(void) {
    int fd[2];
    char buf;
    pipe(fd);
    if (fork() == 0) {
        close(fd[1]);
        read(fd[0], &buf, 1);
        close(fd[0]);
        _exit(0);
    }
    close(fd[0]);
    write(fd[1], "a", 1);
    close(fd[1]);
    return 0;
}

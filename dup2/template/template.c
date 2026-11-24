#include <unistd.h>

int main(void) {
    int fd[2];
    pipe(fd);
    if (fork() == 0) {
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        char *args[] = {"./hijo", NULL};
        execv("./hijo", args);
        _exit(1);
    }
    close(fd[0]);
    write(fd[1], "a", 1);
    close(fd[1]);
    return 0;
}

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(void) {
    char buf;
    mkfifo("/tmp/t_fifo", 0666);
    if (fork() == 0) {
        int fd = open("/tmp/t_fifo", O_RDONLY);
        read(fd, &buf, 1);
        close(fd);
        _exit(0);
    }
    int fd = open("/tmp/t_fifo", O_WRONLY);
    write(fd, "a", 1);
    close(fd);
    unlink("/tmp/t_fifo");
    return 0;
}

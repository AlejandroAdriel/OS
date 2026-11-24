#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();
    if (pid == 0) {
        _exit(0);
    }
    wait(NULL);
    return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(void) {
    // [TECNICA: FORK] Bifurcacion
    pid_t pid = fork();

    if (pid == 0) {
        int dev_null = open("/dev/null", O_WRONLY);
        // [TECNICA: DUP2] Redireccion de stdout exclusivamente en el proceso hijo
        dup2(dev_null, STDOUT_FILENO);
        close(dev_null);
        printf("Este mensaje no se vera porque stdout fue a /dev/null\n");
        _exit(0);
    } else {
        // [TECNICA: FORK] Padre espera
        wait(NULL);
        printf("[DUP2 + FORK] Padre preserva su stdout mientras hijo uso dup2\n");
    }
    return 0;
}

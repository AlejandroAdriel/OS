#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int pfd[2];
    // [TECNICA: PIPE] Creacion de tuberia anonima
    pipe(pfd);

    // [TECNICA: FORK] Clonacion de proceso
    pid_t pid = fork();

    if (pid == 0) {
        close(pfd[1]);
        char buf[32];
        // [TECNICA: PIPE] Hijo lee datos del padre
        int n = read(pfd[0], buf, sizeof(buf)-1);
        buf[n] = '\0';
        close(pfd[0]);
        printf("[PIPE + FORK] Proceso hijo leyo: %s\n", buf);
        _exit(0);
    } else {
        close(pfd[0]);
        // [TECNICA: PIPE] Padre envia datos al hijo
        write(pfd[1], "saludo_pipe_a_hijo", 18);
        close(pfd[1]);
        // [TECNICA: FORK] Espera del hijo
        wait(NULL);
    }
    return 0;
}

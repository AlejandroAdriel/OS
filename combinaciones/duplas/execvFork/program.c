#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    // [TECNICA: FORK] Clonacion clasica de proceso
    pid_t pid = fork();

    if (pid == 0) {
        printf("[EXECV + FORK] Hijo ejecuta nuevo programa...\n");
        char *args[] = {"./hijo", NULL};
        // [TECNICA: EXECV] Hijo cambia su ejecutable
        execv("./hijo", args);
        _exit(1);
    } else {
        // [TECNICA: FORK] Padre espera a que el nuevo binario finalice
        wait(NULL);
        printf("[EXECV + FORK] Padre detecto finalizacion del binario hijo\n");
    }
    return 0;
}

#include <stdio.h>
#include <unistd.h>

int main(void) {
    int pfd[2];
    // [TECNICA: PIPE] Creacion de tuberia anonima
    pipe(pfd);
    write(pfd[1], "datos_para_execv", 16);

    printf("[PIPE + EXECV] Pipe listo con datos. Ejecutando execv...\n");
    char *args[] = {"./hijo", NULL};
    // [TECNICA: EXECV] Reemplazo de memoria (el descriptor pfd[0] queda abierto)
    execv("./hijo", args);
    return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

// [TECNICA: SIGNALIZACION] Manejador que decide invocar execv
void handleSignal(int sig) {
    printf("[SIGNALIZACION + EXECV] Senal %d capturada. Reemplazando imagen de memoria...\n", sig);
    char *args[] = {"./hijo", NULL};
    // [TECNICA: EXECV] Reemplazo de la imagen del proceso actual
    execv("./hijo", args);
    perror("execv");
}

int main(void) {
    // [TECNICA: SIGNALIZACION] Registro del manejador
    signal(SIGUSR1, handleSignal);

    // [TECNICA: SIGNALIZACION] Emision de senal
    raise(SIGUSR1);
    return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

int backup_stdout;

// [TECNICA: SIGNALIZACION] Manejador que restaura la salida estandar redirigida
void handleSignal(int sig) {
    // [TECNICA: DUP2] Restauracion del descriptor stdout original
    dup2(backup_stdout, STDOUT_FILENO);
    printf("[SIGNALIZACION + DUP2] Senal %d recibida: stdout restaurado via dup2\n", sig);
}

int main(void) {
    backup_stdout = dup(STDOUT_FILENO);
    int dev_null = open("/dev/null", O_WRONLY);

    // [TECNICA: DUP2] Redireccion de stdout hacia /dev/null
    dup2(dev_null, STDOUT_FILENO);
    close(dev_null);

    // [TECNICA: SIGNALIZACION] Registro de senal
    signal(SIGUSR1, handleSignal);

    // [TECNICA: SIGNALIZACION] Disparo de senal para disparar el manejador restaurador
    raise(SIGUSR1);

    close(backup_stdout);
    return 0;
}

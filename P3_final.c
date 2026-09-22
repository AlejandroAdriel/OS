#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// =========================================================================
// PASO 7: EXECV (Destino Final de la Cadena Secuencial)
// Este programa es cargado vía execv() y lee desde STDIN_FILENO.
// Gracias al dup2() del Paso 6, STDIN_FILENO está conectado al Pipe Anónimo,
// el cual contenía los datos procedentes de la FIFO, la Cola y la Señal.
// =========================================================================
int main(void) {
    printf("[P3_FINAL] Programa ejecutable independiente cargado vía execv.\n");
    printf("[P3_FINAL] PID: %d | Leyendo desde STDIN_FILENO (conectado al Pipe vía dup2)...\n\n", getpid());

    char buffer[1024];
    ssize_t n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);

    if (n > 0) {
        buffer[n] = '\0';
        printf("===================================================================\n");
        printf("  RESULTADO FINAL DE LA SEGUIDILLA DOMINÓ (7 TÉCNICAS POSIX):\n");
        printf("===================================================================\n");
        printf("  %s\n", buffer);
        printf("===================================================================\n\n");
    } else {
        perror("[P3_FINAL] Error al leer de la entrada estándar");
    }

    return 0;
}
